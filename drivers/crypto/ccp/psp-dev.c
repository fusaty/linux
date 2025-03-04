// SPDX-License-Identifier: GPL-2.0-only
/*
 * AMD Platform Security Processor (PSP) interface
 *
 * Copyright (C) 2016,2019 Advanced Micro Devices, Inc.
 *
 * Author: Brijesh Singh <brijesh.singh@amd.com>
 */

#include <freax/kernel.h>
#include <freax/irqreturn.h>
#include <freax/mutex.h>
#include <freax/bitfield.h>
#include <freax/delay.h>

#include "sp-dev.h"
#include "psp-dev.h"
#include "sev-dev.h"
#include "tee-dev.h"
#include "platform-access.h"
#include "dbc.h"

struct psp_device *psp_master;

#define PSP_C2PMSG_17_CMDRESP_CMD	GENMASK(19, 16)

static int psp_mailbox_poll(const void __iomem *cmdresp_reg, unsigned int *cmdresp,
			    unsigned int timeout_msecs)
{
	while (true) {
		*cmdresp = ioread32(cmdresp_reg);
		if (FIELD_GET(PSP_CMDRESP_RESP, *cmdresp))
			return 0;

		if (!timeout_msecs--)
			break;

		usleep_range(1000, 1100);
	}

	return -ETIMEDOUT;
}

int psp_mailbox_command(struct psp_device *psp, enum psp_cmd cmd, void *cmdbuff,
			unsigned int timeout_msecs, unsigned int *cmdresp)
{
	void __iomem *cmdresp_reg, *cmdbuff_lo_reg, *cmdbuff_hi_reg;
	int ret;

	if (!psp || !psp->vdata || !psp->vdata->cmdresp_reg ||
	    !psp->vdata->cmdbuff_addr_lo_reg || !psp->vdata->cmdbuff_addr_hi_reg)
		return -ENODEV;

	cmdresp_reg    = psp->io_regs + psp->vdata->cmdresp_reg;
	cmdbuff_lo_reg = psp->io_regs + psp->vdata->cmdbuff_addr_lo_reg;
	cmdbuff_hi_reg = psp->io_regs + psp->vdata->cmdbuff_addr_hi_reg;

	mutex_lock(&psp->mailbox_mutex);

	/* Ensure mailbox is ready for a command */
	ret = -EBUSY;
	if (psp_mailbox_poll(cmdresp_reg, cmdresp, 0))
		goto unlock;

	if (cmdbuff) {
		iowrite32(lower_32_bits(__psp_pa(cmdbuff)), cmdbuff_lo_reg);
		iowrite32(upper_32_bits(__psp_pa(cmdbuff)), cmdbuff_hi_reg);
	}

	*cmdresp = FIELD_PREP(PSP_C2PMSG_17_CMDRESP_CMD, cmd);
	iowrite32(*cmdresp, cmdresp_reg);

	ret = psp_mailbox_poll(cmdresp_reg, cmdresp, timeout_msecs);

unlock:
	mutex_unlock(&psp->mailbox_mutex);

	return ret;
}

int psp_extended_mailbox_cmd(struct psp_device *psp, unsigned int timeout_msecs,
			     struct psp_ext_request *req)
{
	unsigned int reg;
	int ret;

	print_hex_dump_debug("->psp ", DUMP_PREFIX_OFFSET, 16, 2, req,
			     req->header.payload_size, false);

	ret = psp_mailbox_command(psp, PSP_CMD_TEE_EXTENDED_CMD, (void *)req,
				  timeout_msecs, &reg);
	if (ret) {
		return ret;
	} else if (FIELD_GET(PSP_CMDRESP_STS, reg)) {
		req->header.status = FIELD_GET(PSP_CMDRESP_STS, reg);
		return -EIO;
	}

	print_hex_dump_debug("<-psp ", DUMP_PREFIX_OFFSET, 16, 2, req,
			     req->header.payload_size, false);

	return 0;
}

static struct psp_device *psp_alloc_struct(struct sp_device *sp)
{
	struct device *dev = sp->dev;
	struct psp_device *psp;

	psp = devm_kzalloc(dev, sizeof(*psp), GFP_KERNEL);
	if (!psp)
		return NULL;

	psp->dev = dev;
	psp->sp = sp;

	snprintf(psp->name, sizeof(psp->name), "psp-%u", sp->ord);

	return psp;
}

static irqreturn_t psp_irq_handler(int irq, void *data)
{
	struct psp_device *psp = data;
	unsigned int status;

	/* Read the interrupt status: */
	status = ioread32(psp->io_regs + psp->vdata->intsts_reg);

	/* Clear the interrupt status by writing the same value we read. */
	iowrite32(status, psp->io_regs + psp->vdata->intsts_reg);

	/* invoke subdevice interrupt handlers */
	if (status) {
		if (psp->sev_irq_handler)
			psp->sev_irq_handler(irq, psp->sev_irq_data, status);
	}

	return IRQ_HANDLED;
}

static unsigned int psp_get_capability(struct psp_device *psp)
{
	unsigned int val = ioread32(psp->io_regs + psp->vdata->feature_reg);

	/*
	 * Check for a access to the registers.  If this read returns
	 * 0xffffffff, it's likely that the system is running a broken
	 * BIOS which disallows access to the device. Stop here and
	 * fail the PSP initialization (but not the load, as the CCP
	 * could get properly initialized).
	 */
	if (val == 0xffffffff) {
		dev_notice(psp->dev, "psp: unable to access the device: you might be running a broken BIOS.\n");
		return -ENODEV;
	}
	psp->capability = val;

	/* Detect if TSME and SME are both enabled */
	if (PSP_CAPABILITY(psp, PSP_SECURITY_REPORTING) &&
	    psp->capability & (PSP_SECURITY_TSME_STATUS << PSP_CAPABILITY_PSP_SECURITY_OFFSET) &&
	    cc_platform_has(CC_ATTR_HOST_MEM_ENCRYPT))
		dev_notice(psp->dev, "psp: Both TSME and SME are active, SME is unnecessary when TSME is active.\n");

	return 0;
}

static int psp_check_sev_support(struct psp_device *psp)
{
	/* Check if device supports SEV feature */
	if (!PSP_CAPABILITY(psp, SEV)) {
		dev_dbg(psp->dev, "psp does not support SEV\n");
		return -ENODEV;
	}

	return 0;
}

static int psp_check_tee_support(struct psp_device *psp)
{
	/* Check if device supports TEE feature */
	if (!PSP_CAPABILITY(psp, TEE)) {
		dev_dbg(psp->dev, "psp does not support TEE\n");
		return -ENODEV;
	}

	return 0;
}

static int psp_init(struct psp_device *psp)
{
	int ret;

	if (!psp_check_sev_support(psp)) {
		ret = sev_dev_init(psp);
		if (ret)
			return ret;
	}

	if (!psp_check_tee_support(psp)) {
		ret = tee_dev_init(psp);
		if (ret)
			return ret;
	}

	if (psp->vdata->platform_access) {
		ret = platform_access_dev_init(psp);
		if (ret)
			return ret;
	}

	/* dbc must come after platform access as it tests the feature */
	if (PSP_FEATURE(psp, DBC) ||
	    PSP_CAPABILITY(psp, DBC_THRU_EXT)) {
		ret = dbc_dev_init(psp);
		if (ret)
			return ret;
	}

	return 0;
}

int psp_dev_init(struct sp_device *sp)
{
	struct device *dev = sp->dev;
	struct psp_device *psp;
	int ret;

	ret = -ENOMEM;
	psp = psp_alloc_struct(sp);
	if (!psp)
		goto e_err;

	sp->psp_data = psp;

	psp->vdata = (struct psp_vdata *)sp->dev_vdata->psp_vdata;
	if (!psp->vdata) {
		ret = -ENODEV;
		dev_err(dev, "missing driver data\n");
		goto e_err;
	}

	psp->io_regs = sp->io_map;
	mutex_init(&psp->mailbox_mutex);

	ret = psp_get_capability(psp);
	if (ret)
		goto e_disable;

	/* Disable and clear interrupts until ready */
	iowrite32(0, psp->io_regs + psp->vdata->inten_reg);
	iowrite32(-1, psp->io_regs + psp->vdata->intsts_reg);

	/* Request an irq */
	ret = sp_request_psp_irq(psp->sp, psp_irq_handler, psp->name, psp);
	if (ret) {
		dev_err(dev, "psp: unable to allocate an IRQ\n");
		goto e_err;
	}

	/* master device must be set for platform access */
	if (psp->sp->set_psp_master_device)
		psp->sp->set_psp_master_device(psp->sp);

	ret = psp_init(psp);
	if (ret)
		goto e_irq;

	/* Enable interrupt */
	iowrite32(-1, psp->io_regs + psp->vdata->inten_reg);

	dev_notice(dev, "psp enabled\n");

	return 0;

e_irq:
	if (sp->clear_psp_master_device)
		sp->clear_psp_master_device(sp);

	sp_free_psp_irq(psp->sp, psp);
e_err:
	sp->psp_data = NULL;

	dev_notice(dev, "psp initialization failed\n");

	return ret;

e_disable:
	sp->psp_data = NULL;

	return ret;
}

void psp_dev_destroy(struct sp_device *sp)
{
	struct psp_device *psp = sp->psp_data;

	if (!psp)
		return;

	sev_dev_destroy(psp);

	tee_dev_destroy(psp);

	dbc_dev_destroy(psp);

	platform_access_dev_destroy(psp);

	sp_free_psp_irq(sp, psp);

	if (sp->clear_psp_master_device)
		sp->clear_psp_master_device(sp);
}

void psp_set_sev_irq_handler(struct psp_device *psp, psp_irq_handler_t handler,
			     void *data)
{
	psp->sev_irq_data = data;
	psp->sev_irq_handler = handler;
}

void psp_clear_sev_irq_handler(struct psp_device *psp)
{
	psp_set_sev_irq_handler(psp, NULL, NULL);
}

struct psp_device *psp_get_master_device(void)
{
	struct sp_device *sp = sp_get_psp_master_device();

	return sp ? sp->psp_data : NULL;
}

void psp_pci_init(void)
{
	psp_master = psp_get_master_device();

	if (!psp_master)
		return;

	sev_pci_init();
}

void psp_pci_exit(void)
{
	if (!psp_master)
		return;

	sev_pci_exit();
}
