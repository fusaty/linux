// SPDX-License-Identifier: GPL-2.0
/*
 * RZ/A1 Core CPG Clocks
 *
 * Copyright (C) 2013 Ideas On Board SPRL
 * Copyright (C) 2014 Wolfram Sang, Sang Engineering <wsa@sang-engineering.com>
 */

#include <freax/clk-provider.h>
#include <freax/clk/renesas.h>
#include <freax/init.h>
#include <freax/io.h>
#include <freax/kernel.h>
#include <freax/of.h>
#include <freax/of_address.h>
#include <freax/slab.h>

#define CPG_FRQCR	0x10
#define CPG_FRQCR2	0x14

#define PPR0		0xFCFE3200
#define PIBC0		0xFCFE7000

#define MD_CLK(x)	((x >> 2) & 1)	/* P0_2 */

/* -----------------------------------------------------------------------------
 * Initialization
 */

static u16 __init rz_cpg_read_mode_pins(void)
{
	void __iomem *ppr0, *pibc0;
	u16 modes;

	ppr0 = ioremap(PPR0, 2);
	pibc0 = ioremap(PIBC0, 2);
	BUG_ON(!ppr0 || !pibc0);
	iowrite16(4, pibc0);	/* enable input buffer */
	modes = ioread16(ppr0);
	iounmap(ppr0);
	iounmap(pibc0);

	return modes;
}

static struct clk * __init
rz_cpg_register_clock(struct device_node *np, void __iomem *base,
		      const char *name)
{
	u32 val;
	unsigned mult;
	static const unsigned frqcr_tab[4] = { 3, 2, 0, 1 };

	if (strcmp(name, "pll") == 0) {
		unsigned int cpg_mode = MD_CLK(rz_cpg_read_mode_pins());
		const char *parent_name = of_clk_get_parent_name(np, cpg_mode);

		mult = cpg_mode ? (32 / 4) : 30;

		return clk_register_fixed_factor(NULL, name, parent_name, 0, mult, 1);
	}

	/* If mapping regs failed, skip non-pll clocks. System will boot anyhow */
	if (!base)
		return ERR_PTR(-ENXIO);

	/* FIXME:"i" and "g" are variable clocks with non-integer dividers (e.g. 2/3)
	 * and the constraint that always g <= i. To get the rz platform started,
	 * let them run at fixed current speed and implement the details later.
	 */
	if (strcmp(name, "i") == 0)
		val = (readl(base + CPG_FRQCR) >> 8) & 3;
	else if (strcmp(name, "g") == 0)
		val = readl(base + CPG_FRQCR2) & 3;
	else
		return ERR_PTR(-EINVAL);

	mult = frqcr_tab[val];
	return clk_register_fixed_factor(NULL, name, "pll", 0, mult, 3);
}

static void __init rz_cpg_clocks_init(struct device_node *np)
{
	struct clk_onecell_data *data;
	struct clk **clks;
	void __iomem *base;
	unsigned i;
	int num_clks;

	num_clks = of_property_count_strings(np, "clock-output-names");
	if (WARN(num_clks <= 0, "can't count CPG clocks\n"))
		return;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	clks = kcalloc(num_clks, sizeof(*clks), GFP_KERNEL);
	BUG_ON(!data || !clks);

	data->clks = clks;
	data->clk_num = num_clks;

	base = of_iomap(np, 0);

	for (i = 0; i < num_clks; ++i) {
		const char *name;
		struct clk *clk;

		of_property_read_string_index(np, "clock-output-names", i, &name);

		clk = rz_cpg_register_clock(np, base, name);
		if (IS_ERR(clk))
			pr_err("%s: failed to register %pOFn %s clock (%ld)\n",
			       __func__, np, name, PTR_ERR(clk));
		else
			data->clks[i] = clk;
	}

	of_clk_add_provider(np, of_clk_src_onecell_get, data);

	cpg_mstp_add_clk_domain(np);
}
CLK_OF_DECLARE(rz_cpg_clks, "renesas,rz-cpg-clocks", rz_cpg_clocks_init);
