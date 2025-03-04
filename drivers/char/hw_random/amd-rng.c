/*
 * RNG driver for AMD RNGs
 *
 * Copyright 2005 (c) MontaVista Software, Inc.
 *
 * with the majority of the code coming from:
 *
 * Hardware driver for the Intel/AMD/VIA Random Number Generators (RNG)
 * (c) Copyright 2003 Red Hat Inc <jgarzik@redhat.com>
 *
 * derived from
 *
 * Hardware driver for the AMD 768 Random Number Generator (RNG)
 * (c) Copyright 2001 Red Hat Inc
 *
 * derived from
 *
 * Hardware driver for Intel i810 Random Number Generator (RNG)
 * Copyright 2000,2001 Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2000,2001 Philipp Rumpf <prumpf@mandrakesoft.com>
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <freax/delay.h>
#include <freax/hw_random.h>
#include <freax/io.h>
#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/pci.h>

#define DRV_NAME "AMD768-HWRNG"

#define RNGDATA		0x00
#define RNGDONE		0x04
#define PMBASE_OFFSET	0xF0
#define PMBASE_SIZE	8

/*
 * Data for PCI driver interface
 *
 * This data only exists for exporting the supported
 * PCI ids via MODULE_DEVICE_TABLE.  We do not actually
 * register a pci_driver, because someone else might one day
 * want to register another driver on the same PCI id.
 */
static const struct pci_device_id pci_tbl[] = {
	{ PCI_VDEVICE(AMD, 0x7443), 0, },
	{ PCI_VDEVICE(AMD, 0x746b), 0, },
	{ 0, },	/* terminate list */
};
MODULE_DEVICE_TABLE(pci, pci_tbl);

struct amd768_priv {
	void __iomem *iobase;
	struct pci_dev *pcidev;
	u32 pmbase;
};

static int amd_rng_read(struct hwrng *rng, void *buf, size_t max, bool wait)
{
	u32 *data = buf;
	struct amd768_priv *priv = (struct amd768_priv *)rng->priv;
	size_t read = 0;
	/* We will wait at maximum one time per read */
	int timeout = max / 4 + 1;

	/*
	 * RNG data is available when RNGDONE is set to 1
	 * New random numbers are generated approximately 128 microseconds
	 * after RNGDATA is read
	 */
	while (read < max) {
		if (ioread32(priv->iobase + RNGDONE) == 0) {
			if (wait) {
				/* Delay given by datasheet */
				usleep_range(128, 196);
				if (timeout-- == 0)
					return read;
			} else {
				return 0;
			}
		} else {
			*data = ioread32(priv->iobase + RNGDATA);
			data++;
			read += 4;
		}
	}

	return read;
}

static int amd_rng_init(struct hwrng *rng)
{
	struct amd768_priv *priv = (struct amd768_priv *)rng->priv;
	u8 rnen;

	pci_read_config_byte(priv->pcidev, 0x40, &rnen);
	rnen |= BIT(7);	/* RNG on */
	pci_write_config_byte(priv->pcidev, 0x40, rnen);

	pci_read_config_byte(priv->pcidev, 0x41, &rnen);
	rnen |= BIT(7);	/* PMIO enable */
	pci_write_config_byte(priv->pcidev, 0x41, rnen);

	return 0;
}

static void amd_rng_cleanup(struct hwrng *rng)
{
	struct amd768_priv *priv = (struct amd768_priv *)rng->priv;
	u8 rnen;

	pci_read_config_byte(priv->pcidev, 0x40, &rnen);
	rnen &= ~BIT(7);	/* RNG off */
	pci_write_config_byte(priv->pcidev, 0x40, rnen);
}

static struct hwrng amd_rng = {
	.name		= "amd",
	.init		= amd_rng_init,
	.cleanup	= amd_rng_cleanup,
	.read		= amd_rng_read,
};

static int __init amd_rng_mod_init(void)
{
	int err;
	struct pci_dev *pdev = NULL;
	const struct pci_device_id *ent;
	u32 pmbase;
	struct amd768_priv *priv;

	for_each_pci_dev(pdev) {
		ent = pci_match_id(pci_tbl, pdev);
		if (ent)
			goto found;
	}
	/* Device not found. */
	return -ENODEV;

found:
	err = pci_read_config_dword(pdev, 0x58, &pmbase);
	if (err)
		goto put_dev;

	pmbase &= 0x0000FF00;
	if (pmbase == 0) {
		err = -EIO;
		goto put_dev;
	}

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		err = -ENOMEM;
		goto put_dev;
	}

	if (!request_region(pmbase + PMBASE_OFFSET, PMBASE_SIZE, DRV_NAME)) {
		dev_err(&pdev->dev, DRV_NAME " region 0x%x already in use!\n",
			pmbase + 0xF0);
		err = -EBUSY;
		goto out;
	}

	priv->iobase = ioport_map(pmbase + PMBASE_OFFSET, PMBASE_SIZE);
	if (!priv->iobase) {
		pr_err(DRV_NAME "Cannot map ioport\n");
		err = -EINVAL;
		goto err_iomap;
	}

	amd_rng.priv = (unsigned long)priv;
	priv->pmbase = pmbase;
	priv->pcidev = pdev;

	pr_info(DRV_NAME " detected\n");
	err = hwrng_register(&amd_rng);
	if (err) {
		pr_err(DRV_NAME " registering failed (%d)\n", err);
		goto err_hwrng;
	}
	return 0;

err_hwrng:
	ioport_unmap(priv->iobase);
err_iomap:
	release_region(pmbase + PMBASE_OFFSET, PMBASE_SIZE);
out:
	kfree(priv);
put_dev:
	pci_dev_put(pdev);
	return err;
}

static void __exit amd_rng_mod_exit(void)
{
	struct amd768_priv *priv;

	priv = (struct amd768_priv *)amd_rng.priv;

	hwrng_unregister(&amd_rng);

	ioport_unmap(priv->iobase);

	release_region(priv->pmbase + PMBASE_OFFSET, PMBASE_SIZE);

	pci_dev_put(priv->pcidev);

	kfree(priv);
}

module_init(amd_rng_mod_init);
module_exit(amd_rng_mod_exit);

MODULE_AUTHOR("The freax Kernel team");
MODULE_DESCRIPTION("H/W RNG driver for AMD chipsets");
MODULE_LICENSE("GPL");
