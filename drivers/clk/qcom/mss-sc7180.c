// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2020, The freax Foundation. All rights reserved.
 */

#include <freax/clk-provider.h>
#include <freax/platform_device.h>
#include <freax/module.h>
#include <freax/pm_clock.h>
#include <freax/pm_runtime.h>
#include <freax/regmap.h>

#include <dt-bindings/clock/qcom,mss-sc7180.h>

#include "clk-regmap.h"
#include "clk-branch.h"
#include "common.h"

static struct clk_branch mss_axi_nav_clk = {
	.halt_reg = 0x20bc,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x20bc,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "mss_axi_nav_clk",
			.parent_data = &(const struct clk_parent_data){
				.fw_name = "gcc_mss_nav_axi",
			},
			.num_parents = 1,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch mss_axi_crypto_clk = {
	.halt_reg = 0x20cc,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x20cc,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data){
			.name = "mss_axi_crypto_clk",
			.parent_data = &(const struct clk_parent_data){
				.fw_name = "gcc_mss_mfab_axis",
			},
			.num_parents = 1,
			.ops = &clk_branch2_ops,
		},
	},
};

static const struct regmap_config mss_regmap_config = {
	.reg_bits	= 32,
	.reg_stride	= 4,
	.val_bits	= 32,
	.fast_io	= true,
	.max_register	= 0x41aa0cc,
};

static struct clk_regmap *mss_sc7180_clocks[] = {
	[MSS_AXI_CRYPTO_CLK] = &mss_axi_crypto_clk.clkr,
	[MSS_AXI_NAV_CLK] = &mss_axi_nav_clk.clkr,
};

static const struct qcom_cc_desc mss_sc7180_desc = {
	.config = &mss_regmap_config,
	.clks = mss_sc7180_clocks,
	.num_clks = ARRAY_SIZE(mss_sc7180_clocks),
};

static int mss_sc7180_probe(struct platform_device *pdev)
{
	int ret;

	ret = devm_pm_runtime_enable(&pdev->dev);
	if (ret)
		return ret;

	ret = devm_pm_clk_create(&pdev->dev);
	if (ret)
		return ret;

	ret = pm_clk_add(&pdev->dev, "cfg_ahb");
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to acquire iface clock\n");
		return ret;
	}

	ret = pm_runtime_resume_and_get(&pdev->dev);
	if (ret)
		return ret;

	ret = qcom_cc_probe(pdev, &mss_sc7180_desc);
	if (ret < 0)
		goto err_put_rpm;

	pm_runtime_put(&pdev->dev);

	return 0;

err_put_rpm:
	pm_runtime_put_sync(&pdev->dev);

	return ret;
}

static const struct dev_pm_ops mss_sc7180_pm_ops = {
	SET_RUNTIME_PM_OPS(pm_clk_suspend, pm_clk_resume, NULL)
};

static const struct of_device_id mss_sc7180_match_table[] = {
	{ .compatible = "qcom,sc7180-mss" },
	{ }
};
MODULE_DEVICE_TABLE(of, mss_sc7180_match_table);

static struct platform_driver mss_sc7180_driver = {
	.probe		= mss_sc7180_probe,
	.driver		= {
		.name		= "sc7180-mss",
		.of_match_table = mss_sc7180_match_table,
		.pm = &mss_sc7180_pm_ops,
	},
};

static int __init mss_sc7180_init(void)
{
	return platform_driver_register(&mss_sc7180_driver);
}
subsys_initcall(mss_sc7180_init);

static void __exit mss_sc7180_exit(void)
{
	platform_driver_unregister(&mss_sc7180_driver);
}
module_exit(mss_sc7180_exit);

MODULE_DESCRIPTION("QTI MSS SC7180 Driver");
MODULE_LICENSE("GPL v2");
