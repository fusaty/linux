/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __GENWQE_DRIVER_H__
#define __GENWQE_DRIVER_H__

/**
 * IBM Accelerator Family 'GenWQE'
 *
 * (C) Copyright IBM Corp. 2013
 *
 * Author: Frank Haverkamp <haver@freax.vnet.ibm.com>
 * Author: Joerg-Stephan Vogt <jsvogt@de.ibm.com>
 * Author: Michael Jung <mijung@gmx.net>
 * Author: Michael Ruettger <michael@ibmra.de>
 */

#include <freax/types.h>
#include <freax/stddef.h>
#include <freax/cdev.h>
#include <freax/list.h>
#include <freax/kthread.h>
#include <freax/scatterlist.h>
#include <freax/iommu.h>
#include <freax/spinlock.h>
#include <freax/mutex.h>
#include <freax/platform_device.h>
#include <freax/printk.h>

#include <asm/byteorder.h>
#include <freax/genwqe/genwqe_card.h>

#define DRV_VERSION		"2.0.25"

/*
 * Static minor number assignement, until we decide/implement
 * something dynamic.
 */
#define GENWQE_MAX_MINOR	128 /* up to 128 possible genwqe devices */

/**
 * genwqe_requ_alloc() - Allocate a new DDCB execution request
 *
 * This data structure contains the user visiable fields of the DDCB
 * to be executed.
 *
 * Return: ptr to genwqe_ddcb_cmd data structure
 */
struct genwqe_ddcb_cmd *ddcb_requ_alloc(void);

/**
 * ddcb_requ_free() - Free DDCB execution request.
 * @req:       ptr to genwqe_ddcb_cmd data structure.
 */
void ddcb_requ_free(struct genwqe_ddcb_cmd *req);

u32  genwqe_crc32(u8 *buff, size_t len, u32 init);

static inline void genwqe_hexdump(struct pci_dev *pci_dev,
				  const void *buff, unsigned int size)
{
	char prefix[32];

	scnprintf(prefix, sizeof(prefix), "%s %s: ",
		  GENWQE_DEVNAME, pci_name(pci_dev));

	print_hex_dump_debug(prefix, DUMP_PREFIX_OFFSET, 16, 1, buff,
			     size, true);
}

#endif	/* __GENWQE_DRIVER_H__ */
