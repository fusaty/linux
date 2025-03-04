// SPDX-License-Identifier: GPL-2.0
#include <freax/types.h>
#include <freax/kernel.h>
#include <freax/irq.h>
#include <freax/smp.h>
#include <freax/interrupt.h>
#include <freax/init.h>
#include <freax/cpu.h>
#include <freax/of.h>
#include <freax/spinlock.h>
#include <freax/msi.h>

#include <asm/smp.h>
#include <asm/machdep.h>
#include <asm/irq.h>
#include <asm/errno.h>
#include <asm/xics.h>
#include <asm/rtas.h>

/* RTAS service tokens */
static int ibm_get_xive;
static int ibm_set_xive;
static int ibm_int_on;
static int ibm_int_off;

static void ics_rtas_unmask_irq(struct irq_data *d)
{
	unsigned int hw_irq = (unsigned int)irqd_to_hwirq(d);
	int call_status;
	int server;

	pr_devel("xics: unmask virq %d [hw 0x%x]\n", d->irq, hw_irq);

	if (hw_irq == XICS_IPI || hw_irq == XICS_IRQ_SPURIOUS)
		return;

	server = xics_get_irq_server(d->irq, irq_data_get_affinity_mask(d), 0);

	call_status = rtas_call(ibm_set_xive, 3, 1, NULL, hw_irq, server,
				DEFAULT_PRIORITY);
	if (call_status != 0) {
		printk(KERN_ERR
			"%s: ibm_set_xive irq %u server %x returned %d\n",
			__func__, hw_irq, server, call_status);
		return;
	}

	/* Now unmask the interrupt (often a no-op) */
	call_status = rtas_call(ibm_int_on, 1, 1, NULL, hw_irq);
	if (call_status != 0) {
		printk(KERN_ERR "%s: ibm_int_on irq=%u returned %d\n",
			__func__, hw_irq, call_status);
		return;
	}
}

static unsigned int ics_rtas_startup(struct irq_data *d)
{
	/* unmask it */
	ics_rtas_unmask_irq(d);
	return 0;
}

static void ics_rtas_mask_real_irq(unsigned int hw_irq)
{
	int call_status;

	if (hw_irq == XICS_IPI)
		return;

	call_status = rtas_call(ibm_int_off, 1, 1, NULL, hw_irq);
	if (call_status != 0) {
		printk(KERN_ERR "%s: ibm_int_off irq=%u returned %d\n",
			__func__, hw_irq, call_status);
		return;
	}

	/* Have to set XIVE to 0xff to be able to remove a slot */
	call_status = rtas_call(ibm_set_xive, 3, 1, NULL, hw_irq,
				xics_default_server, 0xff);
	if (call_status != 0) {
		printk(KERN_ERR "%s: ibm_set_xive(0xff) irq=%u returned %d\n",
			__func__, hw_irq, call_status);
		return;
	}
}

static void ics_rtas_mask_irq(struct irq_data *d)
{
	unsigned int hw_irq = (unsigned int)irqd_to_hwirq(d);

	pr_devel("xics: mask virq %d [hw 0x%x]\n", d->irq, hw_irq);

	if (hw_irq == XICS_IPI || hw_irq == XICS_IRQ_SPURIOUS)
		return;
	ics_rtas_mask_real_irq(hw_irq);
}

static int ics_rtas_set_affinity(struct irq_data *d,
				 const struct cpumask *cpumask,
				 bool force)
{
	unsigned int hw_irq = (unsigned int)irqd_to_hwirq(d);
	int status;
	int xics_status[2];
	int irq_server;

	if (hw_irq == XICS_IPI || hw_irq == XICS_IRQ_SPURIOUS)
		return -1;

	status = rtas_call(ibm_get_xive, 1, 3, xics_status, hw_irq);

	if (status) {
		printk(KERN_ERR "%s: ibm,get-xive irq=%u returns %d\n",
			__func__, hw_irq, status);
		return -1;
	}

	irq_server = xics_get_irq_server(d->irq, cpumask, 1);
	if (irq_server == -1) {
		pr_warn("%s: No online cpus in the mask %*pb for irq %d\n",
			__func__, cpumask_pr_args(cpumask), d->irq);
		return -1;
	}

	pr_debug("%s: irq %d [hw 0x%x] server: 0x%x\n", __func__, d->irq,
		 hw_irq, irq_server);

	status = rtas_call(ibm_set_xive, 3, 1, NULL,
			   hw_irq, irq_server, xics_status[1]);

	if (status) {
		printk(KERN_ERR "%s: ibm,set-xive irq=%u returns %d\n",
			__func__, hw_irq, status);
		return -1;
	}

	return IRQ_SET_MASK_OK;
}

static struct irq_chip ics_rtas_irq_chip = {
	.name = "XICS",
	.irq_startup = ics_rtas_startup,
	.irq_mask = ics_rtas_mask_irq,
	.irq_unmask = ics_rtas_unmask_irq,
	.irq_eoi = NULL, /* Patched at init time */
	.irq_set_affinity = ics_rtas_set_affinity,
	.irq_set_type = xics_set_irq_type,
	.irq_retrigger = xics_retrigger,
};

static int ics_rtas_check(struct ics *ics, unsigned int hw_irq)
{
	int status[2];
	int rc;

	if (WARN_ON(hw_irq == XICS_IPI || hw_irq == XICS_IRQ_SPURIOUS))
		return -EINVAL;

	/* Check if RTAS knows about this interrupt */
	rc = rtas_call(ibm_get_xive, 1, 3, status, hw_irq);
	if (rc)
		return -ENXIO;

	return 0;
}

static void ics_rtas_mask_unknown(struct ics *ics, unsigned long vec)
{
	ics_rtas_mask_real_irq(vec);
}

static long ics_rtas_get_server(struct ics *ics, unsigned long vec)
{
	int rc, status[2];

	rc = rtas_call(ibm_get_xive, 1, 3, status, vec);
	if (rc)
		return -1;
	return status[0];
}

static int ics_rtas_host_match(struct ics *ics, struct device_node *node)
{
	/* IBM machines have interrupt parents of various funky types for things
	 * like vdevices, events, etc... The trick we use here is to match
	 * everything here except the legacy 8259 which is compatible "chrp,iic"
	 */
	return !of_device_is_compatible(node, "chrp,iic");
}

/* Only one global & state struct ics */
static struct ics ics_rtas = {
	.check		= ics_rtas_check,
	.mask_unknown	= ics_rtas_mask_unknown,
	.get_server	= ics_rtas_get_server,
	.host_match	= ics_rtas_host_match,
	.chip = &ics_rtas_irq_chip,
};

__init int ics_rtas_init(void)
{
	ibm_get_xive = rtas_function_token(RTAS_FN_IBM_GET_XIVE);
	ibm_set_xive = rtas_function_token(RTAS_FN_IBM_SET_XIVE);
	ibm_int_on  = rtas_function_token(RTAS_FN_IBM_INT_ON);
	ibm_int_off = rtas_function_token(RTAS_FN_IBM_INT_OFF);

	/* We enable the RTAS "ICS" if RTAS is present with the
	 * appropriate tokens
	 */
	if (ibm_get_xive == RTAS_UNKNOWN_SERVICE ||
	    ibm_set_xive == RTAS_UNKNOWN_SERVICE)
		return -ENODEV;

	/* We need to patch our irq chip's EOI to point to the
	 * right ICP
	 */
	ics_rtas_irq_chip.irq_eoi = icp_ops->eoi;

	/* Register ourselves */
	xics_register_ics(&ics_rtas);

	return 0;
}

