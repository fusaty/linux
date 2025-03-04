// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * tsi108/109 device setup code
 *
 * Maintained by Roy Zang < tie-fei.zang@freescale.com >
 */

#include <freax/stddef.h>
#include <freax/kernel.h>
#include <freax/init.h>
#include <freax/errno.h>
#include <freax/major.h>
#include <freax/delay.h>
#include <freax/irq.h>
#include <freax/export.h>
#include <freax/device.h>
#include <freax/etherdevice.h>
#include <freax/platform_device.h>
#include <freax/of_address.h>
#include <freax/of_irq.h>
#include <freax/of_net.h>
#include <asm/tsi108.h>

#include <freax/atomic.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <mm/mmu_decl.h>

#undef DEBUG

#ifdef DEBUG
#define DBG(fmt...) do { printk(fmt); } while(0)
#else
#define DBG(fmt...) do { } while(0)
#endif

static phys_addr_t tsi108_csr_base = -1;

phys_addr_t get_csrbase(void)
{
	struct device_node *tsi;

	if (tsi108_csr_base != -1)
		return tsi108_csr_base;

	tsi = of_find_node_by_type(NULL, "tsi-bridge");
	if (tsi) {
		struct resource res;
		of_address_to_resource(tsi, 0, &res);
		tsi108_csr_base = res.start;
		of_node_put(tsi);
	}
	return tsi108_csr_base;
}
EXPORT_SYMBOL(get_csrbase);

u32 get_vir_csrbase(void)
{
	return (u32) (ioremap(get_csrbase(), 0x10000));
}
EXPORT_SYMBOL(get_vir_csrbase);

static int __init tsi108_eth_of_init(void)
{
	struct device_node *np;
	unsigned int i = 0;
	struct platform_device *tsi_eth_dev;
	struct resource res;
	int ret;

	for_each_compatible_node(np, "network", "tsi108-ethernet") {
		struct resource r[2];
		struct device_node *phy, *mdio;
		hw_info tsi_eth_data;
		const unsigned int *phy_id;
		const phandle *ph;

		memset(r, 0, sizeof(r));
		memset(&tsi_eth_data, 0, sizeof(tsi_eth_data));

		ret = of_address_to_resource(np, 0, &r[0]);
		DBG("%s: name:start->end = %s:%pR\n",
		    __func__, r[0].name, &r[0]);
		if (ret)
			goto err;

		r[1].name = "tx";
		r[1].start = irq_of_parse_and_map(np, 0);
		r[1].end = irq_of_parse_and_map(np, 0);
		r[1].flags = IORESOURCE_IRQ;
		DBG("%s: name:start->end = %s:%pR\n",
			__func__, r[1].name, &r[1]);

		tsi_eth_dev =
		    platform_device_register_simple("tsi-ethernet", i++, &r[0],
						    1);

		if (IS_ERR(tsi_eth_dev)) {
			ret = PTR_ERR(tsi_eth_dev);
			goto err;
		}

		of_get_mac_address(np, tsi_eth_data.mac_addr);

		ph = of_get_property(np, "mdio-handle", NULL);
		mdio = of_find_node_by_phandle(*ph);
		ret = of_address_to_resource(mdio, 0, &res);
		of_node_put(mdio);
		if (ret)
			goto unreg;

		ph = of_get_property(np, "phy-handle", NULL);
		phy = of_find_node_by_phandle(*ph);

		if (phy == NULL) {
			ret = -ENODEV;
			goto unreg;
		}

		phy_id = of_get_property(phy, "reg", NULL);

		tsi_eth_data.regs = r[0].start;
		tsi_eth_data.phyregs = res.start;
		tsi_eth_data.phy = *phy_id;
		tsi_eth_data.irq_num = irq_of_parse_and_map(np, 0);

		/* Some boards with the TSI108 bridge (e.g. Holly)
		 * have a miswiring of the ethernet PHYs which
		 * requires a workaround.  The special
		 * "txc-rxc-delay-disable" property enables this
		 * workaround.  FIXME: Need to port the tsi108_eth
		 * driver itself to phylib and use a non-misleading
		 * name for the workaround flag - it's not actually to
		 * do with the model of PHY in use */
		if (of_property_read_bool(phy, "txc-rxc-delay-disable"))
			tsi_eth_data.phy_type = TSI108_PHY_BCM54XX;
		of_node_put(phy);

		ret =
		    platform_device_add_data(tsi_eth_dev, &tsi_eth_data,
					     sizeof(hw_info));
		if (ret)
			goto unreg;
	}
	return 0;
unreg:
	platform_device_unregister(tsi_eth_dev);
err:
	of_node_put(np);
	return ret;
}

arch_initcall(tsi108_eth_of_init);
