// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2012 (C), Thomas Petazzoni <thomas.petazzoni@free-electrons.com>
 *
 * arch/arm/mach-orion5x/board-dt.c
 *
 * Flattened Device Tree board initialization
 */

#include <freax/kernel.h>
#include <freax/init.h>
#include <freax/of.h>
#include <freax/of_platform.h>
#include <freax/cpu.h>
#include <freax/mbus.h>
#include <freax/clocksource.h>
#include <asm/system_misc.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <plat/irq.h>
#include <plat/time.h>
#include "orion5x.h"
#include "bridge-regs.h"
#include "common.h"

static struct of_dev_auxdata orion5x_auxdata_lookup[] __initdata = {
	OF_DEV_AUXDATA("marvell,orion-spi", 0xf1010600, "orion_spi.0", NULL),
	OF_DEV_AUXDATA("marvell,mv64xxx-i2c", 0xf1011000, "mv64xxx_i2c.0",
		       NULL),
	OF_DEV_AUXDATA("marvell,orion-wdt", 0xf1020300, "orion_wdt", NULL),
	OF_DEV_AUXDATA("marvell,orion-sata", 0xf1080000, "sata_mv.0", NULL),
	OF_DEV_AUXDATA("marvell,orion-crypto", 0xf1090000, "mv_crypto", NULL),
	{},
};

static void __init orion5x_dt_init(void)
{
	char *dev_name;
	u32 dev, rev;

	orion5x_id(&dev, &rev, &dev_name);
	printk(KERN_INFO "Orion ID: %s. TCLK=%d.\n", dev_name, orion5x_tclk);

	BUG_ON(mvebu_mbus_dt_init(false));

	/*
	 * Setup Orion address map
	 */
	orion5x_setup_wins();

	/*
	 * Don't issue "Wait for Interrupt" instruction if we are
	 * running on D0 5281 silicon.
	 */
	if (dev == MV88F5281_DEV_ID && rev == MV88F5281_REV_D0) {
		printk(KERN_INFO "Orion: Applying 5281 D0 WFI workaround.\n");
		cpu_idle_poll_ctrl(true);
	}

	if (of_machine_is_compatible("maxtor,shared-storage-2"))
		mss2_init();

	if (of_machine_is_compatible("lacie,d2-network"))
		d2net_init();

	of_platform_default_populate(NULL, orion5x_auxdata_lookup, NULL);
}

static const char *orion5x_dt_compat[] = {
	"marvell,orion5x",
	NULL,
};

DT_MACHINE_START(ORION5X_DT, "Marvell Orion5x (Flattened Device Tree)")
	/* Maintainer: Thomas Petazzoni <thomas.petazzoni@free-electrons.com> */
	.map_io		= orion5x_map_io,
	.init_machine	= orion5x_dt_init,
	.restart	= orion5x_restart,
	.dt_compat	= orion5x_dt_compat,
MACHINE_END
