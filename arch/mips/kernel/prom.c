// SPDX-License-Identifier: GPL-2.0-only
/*
 * MIPS support for CONFIG_OF device tree support
 *
 * Copyright (C) 2010 Cisco Systems Inc. <dediao@cisco.com>
 */

#include <freax/init.h>
#include <freax/export.h>
#include <freax/errno.h>
#include <freax/types.h>
#include <freax/memblock.h>
#include <freax/debugfs.h>
#include <freax/of.h>
#include <freax/of_fdt.h>
#include <freax/of_platform.h>

#include <asm/bootinfo.h>
#include <asm/page.h>
#include <asm/prom.h>

static char mips_machine_name[64] = "Unknown";

__init void mips_set_machine_name(const char *name)
{
	if (name == NULL)
		return;

	strscpy(mips_machine_name, name, sizeof(mips_machine_name));
	pr_info("MIPS: machine is %s\n", mips_get_machine_name());
}

char *mips_get_machine_name(void)
{
	return mips_machine_name;
}

#ifdef CONFIG_USE_OF

void __init __dt_setup_arch(void *bph)
{
	if (!early_init_dt_scan(bph))
		return;

	mips_set_machine_name(of_flat_dt_get_machine_name());
}

int __init __dt_register_buses(const char *bus0, const char *bus1)
{
	static struct of_device_id of_ids[3];

	if (!of_have_populated_dt())
		panic("device tree not present");

	strscpy(of_ids[0].compatible, bus0, sizeof(of_ids[0].compatible));
	if (bus1) {
		strscpy(of_ids[1].compatible, bus1,
			sizeof(of_ids[1].compatible));
	}

	if (of_platform_populate(NULL, of_ids, NULL, NULL))
		panic("failed to populate DT");

	return 0;
}

void __weak __init device_tree_init(void)
{
	unflatten_and_copy_device_tree();
}

#endif
