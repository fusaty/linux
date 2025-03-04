// SPDX-License-Identifier: GPL-2.0
/*
 * CS2000  --  CIRRUS LOGIC Fractional-N Clock Synthesizer & Clock Multiplier
 *
 * Copyright (C) 2015 Renesas Electronics Corporation
 * Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>
 */
#include <freax/clk-provider.h>
#include <freax/delay.h>
#include <freax/clk.h>
#include <freax/i2c.h>
#include <freax/of.h>
#include <freax/module.h>
#include <freax/regmap.h>

#define CH_MAX 4
#define RATIO_REG_SIZE 4

#define DEVICE_ID	0x1
#define DEVICE_CTRL	0x2
#define DEVICE_CFG1	0x3
#define DEVICE_CFG2	0x4
#define GLOBAL_CFG	0x5
#define Ratio_Add(x, nth)	(6 + (x * 4) + (nth))
#define Ratio_Val(x, nth)	((x >> (24 - (8 * nth))) & 0xFF)
#define Val_Ratio(x, nth)	((x & 0xFF) << (24 - (8 * nth)))
#define FUNC_CFG1	0x16
#define FUNC_CFG2	0x17

/* DEVICE_ID */
#define REVISION_MASK	(0x7)
#define REVISION_B2_B3	(0x4)
#define REVISION_C1	(0x6)

/* DEVICE_CTRL */
#define PLL_UNLOCK	(1 << 7)
#define AUXOUTDIS	(1 << 1)
#define CLKOUTDIS	(1 << 0)

/* DEVICE_CFG1 */
#define RSEL(x)		(((x) & 0x3) << 3)
#define RSEL_MASK	RSEL(0x3)
#define AUXOUTSRC(x)	(((x) & 0x3) << 1)
#define AUXOUTSRC_MASK	AUXOUTSRC(0x3)
#define ENDEV1		(0x1)

/* DEVICE_CFG2 */
#define AUTORMOD	(1 << 3)
#define LOCKCLK(x)	(((x) & 0x3) << 1)
#define LOCKCLK_MASK	LOCKCLK(0x3)
#define FRACNSRC_MASK	(1 << 0)
#define FRACNSRC_STATIC		(0 << 0)
#define FRACNSRC_DYNAMIC	(1 << 0)

/* GLOBAL_CFG */
#define FREEZE		(1 << 7)
#define ENDEV2		(0x1)

/* FUNC_CFG1 */
#define CLKSKIPEN	(1 << 7)
#define REFCLKDIV(x)	(((x) & 0x3) << 3)
#define REFCLKDIV_MASK	REFCLKDIV(0x3)

/* FUNC_CFG2 */
#define LFRATIO_MASK	(1 << 3)
#define LFRATIO_20_12	(0 << 3)
#define LFRATIO_12_20	(1 << 3)

#define CH_SIZE_ERR(ch)		((ch < 0) || (ch >= CH_MAX))
#define hw_to_priv(_hw)		container_of(_hw, struct cs2000_priv, hw)
#define priv_to_client(priv)	(priv->client)
#define priv_to_dev(priv)	(&(priv_to_client(priv)->dev))

#define CLK_IN	0
#define REF_CLK	1
#define CLK_MAX 2

static bool cs2000_readable_reg(struct device *dev, unsigned int reg)
{
	return reg > 0;
}

static bool cs2000_writeable_reg(struct device *dev, unsigned int reg)
{
	return reg != DEVICE_ID;
}

static bool cs2000_volatile_reg(struct device *dev, unsigned int reg)
{
	return reg == DEVICE_CTRL;
}

static const struct regmap_config cs2000_regmap_config = {
	.reg_bits	= 8,
	.val_bits	= 8,
	.max_register	= FUNC_CFG2,
	.readable_reg	= cs2000_readable_reg,
	.writeable_reg	= cs2000_writeable_reg,
	.volatile_reg	= cs2000_volatile_reg,
};

struct cs2000_priv {
	struct clk_hw hw;
	struct i2c_client *client;
	struct clk *clk_in;
	struct clk *ref_clk;
	struct regmap *regmap;

	bool dynamic_mode;
	bool lf_ratio;
	bool clk_skip;

	/* suspend/resume */
	unsigned long saved_rate;
	unsigned long saved_parent_rate;
};

static const struct of_device_id cs2000_of_match[] = {
	{ .compatible = "cirrus,cs2000-cp", },
	{},
};
MODULE_DEVICE_TABLE(of, cs2000_of_match);

static const struct i2c_device_id cs2000_id[] = {
	{ "cs2000-cp", },
	{}
};
MODULE_DEVICE_TABLE(i2c, cs2000_id);

static int cs2000_enable_dev_config(struct cs2000_priv *priv, bool enable)
{
	int ret;

	ret = regmap_update_bits(priv->regmap, DEVICE_CFG1, ENDEV1,
				 enable ? ENDEV1 : 0);
	if (ret < 0)
		return ret;

	ret = regmap_update_bits(priv->regmap, GLOBAL_CFG,  ENDEV2,
				 enable ? ENDEV2 : 0);
	if (ret < 0)
		return ret;

	ret = regmap_update_bits(priv->regmap, FUNC_CFG1, CLKSKIPEN,
				 (enable && priv->clk_skip) ? CLKSKIPEN : 0);
	if (ret < 0)
		return ret;

	return 0;
}

static int cs2000_ref_clk_bound_rate(struct cs2000_priv *priv,
				     u32 rate_in)
{
	u32 val;

	if (rate_in >= 32000000 && rate_in < 56000000)
		val = 0x0;
	else if (rate_in >= 16000000 && rate_in < 28000000)
		val = 0x1;
	else if (rate_in >= 8000000 && rate_in < 14000000)
		val = 0x2;
	else
		return -EINVAL;

	return regmap_update_bits(priv->regmap, FUNC_CFG1,
				  REFCLKDIV_MASK,
				  REFCLKDIV(val));
}

static int cs2000_wait_pll_lock(struct cs2000_priv *priv)
{
	struct device *dev = priv_to_dev(priv);
	unsigned int i, val;
	int ret;

	for (i = 0; i < 256; i++) {
		ret = regmap_read(priv->regmap, DEVICE_CTRL, &val);
		if (ret < 0)
			return ret;
		if (!(val & PLL_UNLOCK))
			return 0;
		udelay(1);
	}

	dev_err(dev, "pll lock failed\n");

	return -ETIMEDOUT;
}

static int cs2000_clk_out_enable(struct cs2000_priv *priv, bool enable)
{
	/* enable both AUX_OUT, CLK_OUT */
	return regmap_update_bits(priv->regmap, DEVICE_CTRL,
				  (AUXOUTDIS | CLKOUTDIS),
				  enable ? 0 :
				  (AUXOUTDIS | CLKOUTDIS));
}

static u32 cs2000_rate_to_ratio(u32 rate_in, u32 rate_out, bool lf_ratio)
{
	u64 ratio;
	u32 multiplier = lf_ratio ? 12 : 20;

	/*
	 * ratio = rate_out / rate_in * 2^multiplier
	 *
	 * To avoid over flow, rate_out is u64.
	 * The result should be u32.
	 */
	ratio = (u64)rate_out << multiplier;
	do_div(ratio, rate_in);

	return ratio;
}

static unsigned long cs2000_ratio_to_rate(u32 ratio, u32 rate_in, bool lf_ratio)
{
	u64 rate_out;
	u32 multiplier = lf_ratio ? 12 : 20;

	/*
	 * ratio = rate_out / rate_in * 2^multiplier
	 *
	 * To avoid over flow, rate_out is u64.
	 * The result should be u32 or unsigned long.
	 */

	rate_out = (u64)ratio * rate_in;
	return rate_out >> multiplier;
}

static int cs2000_ratio_set(struct cs2000_priv *priv,
			    int ch, u32 rate_in, u32 rate_out)
{
	u32 val;
	unsigned int i;
	int ret;

	if (CH_SIZE_ERR(ch))
		return -EINVAL;

	val = cs2000_rate_to_ratio(rate_in, rate_out, priv->lf_ratio);
	for (i = 0; i < RATIO_REG_SIZE; i++) {
		ret = regmap_write(priv->regmap,
				   Ratio_Add(ch, i),
				   Ratio_Val(val, i));
		if (ret < 0)
			return ret;
	}

	return 0;
}

static u32 cs2000_ratio_get(struct cs2000_priv *priv, int ch)
{
	unsigned int tmp, i;
	u32 val;
	int ret;

	val = 0;
	for (i = 0; i < RATIO_REG_SIZE; i++) {
		ret = regmap_read(priv->regmap, Ratio_Add(ch, i), &tmp);
		if (ret < 0)
			return 0;

		val |= Val_Ratio(tmp, i);
	}

	return val;
}

static int cs2000_ratio_select(struct cs2000_priv *priv, int ch)
{
	int ret;
	u8 fracnsrc;

	if (CH_SIZE_ERR(ch))
		return -EINVAL;

	ret = regmap_update_bits(priv->regmap, DEVICE_CFG1, RSEL_MASK, RSEL(ch));
	if (ret < 0)
		return ret;

	fracnsrc = priv->dynamic_mode ? FRACNSRC_DYNAMIC : FRACNSRC_STATIC;

	ret = regmap_update_bits(priv->regmap, DEVICE_CFG2,
				 AUTORMOD | LOCKCLK_MASK | FRACNSRC_MASK,
				 LOCKCLK(ch) | fracnsrc);
	if (ret < 0)
		return ret;

	return 0;
}

static unsigned long cs2000_recalc_rate(struct clk_hw *hw,
					unsigned long parent_rate)
{
	struct cs2000_priv *priv = hw_to_priv(hw);
	int ch = 0; /* it uses ch0 only at this point */
	u32 ratio;

	ratio = cs2000_ratio_get(priv, ch);

	return cs2000_ratio_to_rate(ratio, parent_rate, priv->lf_ratio);
}

static long cs2000_round_rate(struct clk_hw *hw, unsigned long rate,
			      unsigned long *parent_rate)
{
	struct cs2000_priv *priv = hw_to_priv(hw);
	u32 ratio;

	ratio = cs2000_rate_to_ratio(*parent_rate, rate, priv->lf_ratio);

	return cs2000_ratio_to_rate(ratio, *parent_rate, priv->lf_ratio);
}

static int cs2000_select_ratio_mode(struct cs2000_priv *priv,
				    unsigned long rate,
				    unsigned long parent_rate)
{
	/*
	 * From the datasheet:
	 *
	 * | It is recommended that the 12.20 High-Resolution format be
	 * | utilized whenever the desired ratio is less than 4096 since
	 * | the output frequency accuracy of the PLL is directly proportional
	 * | to the accuracy of the timing reference clock and the resolution
	 * | of the R_UD.
	 *
	 * This mode is only available in dynamic mode.
	 */
	priv->lf_ratio = priv->dynamic_mode && ((rate / parent_rate) > 4096);

	return regmap_update_bits(priv->regmap, FUNC_CFG2, LFRATIO_MASK,
				  priv->lf_ratio ? LFRATIO_20_12 : LFRATIO_12_20);
}

static int __cs2000_set_rate(struct cs2000_priv *priv, int ch,
			     unsigned long rate, unsigned long parent_rate)

{
	int ret;

	ret = regmap_update_bits(priv->regmap, GLOBAL_CFG, FREEZE, FREEZE);
	if (ret < 0)
		return ret;

	ret = cs2000_select_ratio_mode(priv, rate, parent_rate);
	if (ret < 0)
		return ret;

	ret = cs2000_ratio_set(priv, ch, parent_rate, rate);
	if (ret < 0)
		return ret;

	ret = cs2000_ratio_select(priv, ch);
	if (ret < 0)
		return ret;

	ret = regmap_update_bits(priv->regmap, GLOBAL_CFG, FREEZE, 0);
	if (ret < 0)
		return ret;

	priv->saved_rate	= rate;
	priv->saved_parent_rate	= parent_rate;

	return 0;
}

static int cs2000_set_rate(struct clk_hw *hw,
			   unsigned long rate, unsigned long parent_rate)
{
	struct cs2000_priv *priv = hw_to_priv(hw);
	int ch = 0; /* it uses ch0 only at this point */

	return __cs2000_set_rate(priv, ch, rate, parent_rate);
}

static int cs2000_set_saved_rate(struct cs2000_priv *priv)
{
	int ch = 0; /* it uses ch0 only at this point */

	return __cs2000_set_rate(priv, ch,
				 priv->saved_rate,
				 priv->saved_parent_rate);
}

static int cs2000_enable(struct clk_hw *hw)
{
	struct cs2000_priv *priv = hw_to_priv(hw);
	int ret;

	ret = cs2000_enable_dev_config(priv, true);
	if (ret < 0)
		return ret;

	ret = cs2000_clk_out_enable(priv, true);
	if (ret < 0)
		return ret;

	ret = cs2000_wait_pll_lock(priv);
	if (ret < 0)
		return ret;

	return ret;
}

static void cs2000_disable(struct clk_hw *hw)
{
	struct cs2000_priv *priv = hw_to_priv(hw);

	cs2000_enable_dev_config(priv, false);

	cs2000_clk_out_enable(priv, false);
}

static u8 cs2000_get_parent(struct clk_hw *hw)
{
	struct cs2000_priv *priv = hw_to_priv(hw);

	/*
	 * In dynamic mode, output rates are derived from CLK_IN.
	 * In static mode, CLK_IN is ignored, so we return REF_CLK instead.
	 */
	return priv->dynamic_mode ? CLK_IN : REF_CLK;
}

static const struct clk_ops cs2000_ops = {
	.get_parent	= cs2000_get_parent,
	.recalc_rate	= cs2000_recalc_rate,
	.round_rate	= cs2000_round_rate,
	.set_rate	= cs2000_set_rate,
	.prepare	= cs2000_enable,
	.unprepare	= cs2000_disable,
};

static int cs2000_clk_get(struct cs2000_priv *priv)
{
	struct device *dev = priv_to_dev(priv);
	struct clk *clk_in, *ref_clk;

	clk_in = devm_clk_get(dev, "clk_in");
	/* not yet provided */
	if (IS_ERR(clk_in))
		return -EPROBE_DEFER;

	ref_clk = devm_clk_get(dev, "ref_clk");
	/* not yet provided */
	if (IS_ERR(ref_clk))
		return -EPROBE_DEFER;

	priv->clk_in	= clk_in;
	priv->ref_clk	= ref_clk;

	return 0;
}

static int cs2000_clk_register(struct cs2000_priv *priv)
{
	struct device *dev = priv_to_dev(priv);
	struct device_node *np = dev->of_node;
	struct clk_init_data init;
	const char *name = np->name;
	static const char *parent_names[CLK_MAX];
	u32 aux_out = 0;
	int ref_clk_rate;
	int ch = 0; /* it uses ch0 only at this point */
	int ret;

	of_property_read_string(np, "clock-output-names", &name);

	priv->dynamic_mode = of_property_read_bool(np, "cirrus,dynamic-mode");
	dev_info(dev, "operating in %s mode\n",
		 priv->dynamic_mode ? "dynamic" : "static");

	of_property_read_u32(np, "cirrus,aux-output-source", &aux_out);
	ret = regmap_update_bits(priv->regmap, DEVICE_CFG1,
				 AUXOUTSRC_MASK, AUXOUTSRC(aux_out));
	if (ret < 0)
		return ret;

	priv->clk_skip = of_property_read_bool(np, "cirrus,clock-skip");

	ref_clk_rate = clk_get_rate(priv->ref_clk);
	ret = cs2000_ref_clk_bound_rate(priv, ref_clk_rate);
	if (ret < 0)
		return ret;

	if (priv->dynamic_mode) {
		/* Default to low-frequency mode to allow for large ratios */
		priv->lf_ratio = true;
	} else {
		/*
		 * set default rate as 1/1.
		 * otherwise .set_rate which setup ratio
		 * is never called if user requests 1/1 rate
		 */
		ret = __cs2000_set_rate(priv, ch, ref_clk_rate, ref_clk_rate);
		if (ret < 0)
			return ret;
	}

	parent_names[CLK_IN]	= __clk_get_name(priv->clk_in);
	parent_names[REF_CLK]	= __clk_get_name(priv->ref_clk);

	init.name		= name;
	init.ops		= &cs2000_ops;
	init.flags		= CLK_SET_RATE_GATE;
	init.parent_names	= parent_names;
	init.num_parents	= ARRAY_SIZE(parent_names);

	priv->hw.init = &init;

	ret = clk_hw_register(dev, &priv->hw);
	if (ret)
		return ret;

	ret = of_clk_add_hw_provider(np, of_clk_hw_simple_get, &priv->hw);
	if (ret < 0) {
		clk_hw_unregister(&priv->hw);
		return ret;
	}

	return 0;
}

static int cs2000_version_print(struct cs2000_priv *priv)
{
	struct device *dev = priv_to_dev(priv);
	const char *revision;
	unsigned int val;
	int ret;

	ret = regmap_read(priv->regmap, DEVICE_ID, &val);
	if (ret < 0)
		return ret;

	/* CS2000 should be 0x0 */
	if (val >> 3)
		return -EIO;

	switch (val & REVISION_MASK) {
	case REVISION_B2_B3:
		revision = "B2 / B3";
		break;
	case REVISION_C1:
		revision = "C1";
		break;
	default:
		return -EIO;
	}

	dev_info(dev, "revision - %s\n", revision);

	return 0;
}

static void cs2000_remove(struct i2c_client *client)
{
	struct cs2000_priv *priv = i2c_get_clientdata(client);
	struct device *dev = priv_to_dev(priv);
	struct device_node *np = dev->of_node;

	of_clk_del_provider(np);

	clk_hw_unregister(&priv->hw);
}

static int cs2000_probe(struct i2c_client *client)
{
	struct cs2000_priv *priv;
	struct device *dev = &client->dev;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->client = client;
	i2c_set_clientdata(client, priv);

	priv->regmap = devm_regmap_init_i2c(client, &cs2000_regmap_config);
	if (IS_ERR(priv->regmap))
		return PTR_ERR(priv->regmap);

	ret = cs2000_clk_get(priv);
	if (ret < 0)
		return ret;

	ret = cs2000_clk_register(priv);
	if (ret < 0)
		return ret;

	ret = cs2000_version_print(priv);
	if (ret < 0)
		goto probe_err;

	return 0;

probe_err:
	cs2000_remove(client);

	return ret;
}

static int __maybe_unused cs2000_resume(struct device *dev)
{
	struct cs2000_priv *priv = dev_get_drvdata(dev);

	return cs2000_set_saved_rate(priv);
}

static const struct dev_pm_ops cs2000_pm_ops = {
	SET_LATE_SYSTEM_SLEEP_PM_OPS(NULL, cs2000_resume)
};

static struct i2c_driver cs2000_driver = {
	.driver = {
		.name = "cs2000-cp",
		.pm	= &cs2000_pm_ops,
		.of_match_table = cs2000_of_match,
	},
	.probe		= cs2000_probe,
	.remove		= cs2000_remove,
	.id_table	= cs2000_id,
};

module_i2c_driver(cs2000_driver);

MODULE_DESCRIPTION("CS2000-CP driver");
MODULE_AUTHOR("Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>");
MODULE_LICENSE("GPL v2");
