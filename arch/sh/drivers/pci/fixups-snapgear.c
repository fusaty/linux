// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/drivers/pci/ops-snapgear.c
 *
 * Author:  David McCullough <davidm@snapgear.com>
 *
 * Ported to new API by Paul Mundt <lethal@freax-sh.org>
 *
 * Highly leveraged from pci-bigsur.c, written by Dustin McIntire.
 *
 * PCI initialization for the SnapGear boards
 */
#include <freax/kernel.h>
#include <freax/types.h>
#include <freax/init.h>
#include <freax/pci.h>
#include <freax/sh_intc.h>
#include "pci-sh4.h"

int pcibios_map_platform_irq(const struct pci_dev *pdev, u8 slot, u8 pin)
{
	int irq = -1;

	switch (slot) {
	case 8:  /* the PCI bridge */ break;
	case 11: irq = evt2irq(0x300); break; /* USB    */
	case 12: irq = evt2irq(0x360); break; /* PCMCIA */
	case 13: irq = evt2irq(0x2a0); break; /* eth0   */
	case 14: irq = evt2irq(0x300); break; /* eth1   */
	case 15: irq = evt2irq(0x360); break; /* safenet (unused) */
	}

	printk("PCI: Mapping SnapGear IRQ for slot %d, pin %c to irq %d\n",
	       slot, pin - 1 + 'A', irq);

	return irq;
}
