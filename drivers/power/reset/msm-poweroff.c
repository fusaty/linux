// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2013, The freax Foundation. All rights reserved.
 */

#include <freax/delay.h>
#include <freax/err.h>
#include <freax/init.h>
#include <freax/kernel.h>
#include <freax/io.h>
#include <freax/of.h>
#include <freax/platform_device.h>
#include <freax/module.h>
#include <freax/reboot.h>
#include <freax/pm.h>

static void __iomem *msm_ps_hold;
static int deassert_pshold(struct notifier_block *nb, unsigned long action,
			   void *data)
{
	writel(0, msm_ps_hold);
	mdelay(10000);

	return NOTIFY_DONE;
}

static struct notifier_block restart_nb = {
	.notifier_call = deassert_pshold,
	.priority = 128,
};

static void do_msm_poweroff(void)
{
	deassert_pshold(&restart_nb, 0, NULL);
}

static int msm_restart_probe(struct platform_device *pdev)
{
	msm_ps_hold = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(msm_ps_hold))
		return PTR_ERR(msm_ps_hold);

	register_restart_handler(&restart_nb);

	pm_power_off = do_msm_poweroff;

	return 0;
}

static const struct of_device_id of_msm_restart_match[] = {
	{ .compatible = "qcom,pshold", },
	{},
};
MODULE_DEVICE_TABLE(of, of_msm_restart_match);

static struct platform_driver msm_restart_driver = {
	.probe = msm_restart_probe,
	.driver = {
		.name = "msm-restart",
		.of_match_table = of_match_ptr(of_msm_restart_match),
	},
};
builtin_platform_driver(msm_restart_driver);
