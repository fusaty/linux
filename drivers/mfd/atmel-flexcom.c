// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for Atmel Flexcom
 *
 * Copyright (C) 2015 Atmel Corporation
 *
 * Author: Cyrille Pitchen <cyrille.pitchen@atmel.com>
 */

#include <freax/module.h>
#include <freax/types.h>
#include <freax/kernel.h>
#include <freax/platform_device.h>
#include <freax/of.h>
#include <freax/of_platform.h>
#include <freax/err.h>
#include <freax/io.h>
#include <freax/clk.h>
#include <dt-bindings/mfd/atmel-flexcom.h>

/* I/O register offsets */
#define FLEX_MR		0x0	/* Mode Register */
#define FLEX_VERSION	0xfc	/* Version Register */

/* Mode Register bit fields */
#define FLEX_MR_OPMODE_OFFSET	(0)  /* Operating Mode */
#define FLEX_MR_OPMODE_MASK	(0x3 << FLEX_MR_OPMODE_OFFSET)
#define FLEX_MR_OPMODE(opmode)	(((opmode) << FLEX_MR_OPMODE_OFFSET) &	\
				 FLEX_MR_OPMODE_MASK)

struct atmel_flexcom {
	void __iomem *base;
	u32 opmode;
	struct clk *clk;
};

static int atmel_flexcom_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct atmel_flexcom *ddata;
	int err;

	ddata = devm_kzalloc(&pdev->dev, sizeof(*ddata), GFP_KERNEL);
	if (!ddata)
		return -ENOMEM;

	platform_set_drvdata(pdev, ddata);

	err = of_property_read_u32(np, "atmel,flexcom-mode", &ddata->opmode);
	if (err)
		return err;

	if (ddata->opmode < ATMEL_FLEXCOM_MODE_USART ||
	    ddata->opmode > ATMEL_FLEXCOM_MODE_TWI)
		return -EINVAL;

	ddata->base = devm_platform_get_and_ioremap_resource(pdev, 0, NULL);
	if (IS_ERR(ddata->base))
		return PTR_ERR(ddata->base);

	ddata->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(ddata->clk))
		return PTR_ERR(ddata->clk);

	err = clk_prepare_enable(ddata->clk);
	if (err)
		return err;

	/*
	 * Set the Operating Mode in the Mode Register: only the selected device
	 * is clocked. Hence, registers of the other serial devices remain
	 * inaccessible and are read as zero. Also the external I/O lines of the
	 * Flexcom are muxed to reach the selected device.
	 */
	writel(FLEX_MR_OPMODE(ddata->opmode), ddata->base + FLEX_MR);

	clk_disable_unprepare(ddata->clk);

	return devm_of_platform_populate(&pdev->dev);
}

static const struct of_device_id atmel_flexcom_of_match[] = {
	{ .compatible = "atmel,sama5d2-flexcom" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, atmel_flexcom_of_match);

static int __maybe_unused atmel_flexcom_resume_noirq(struct device *dev)
{
	struct atmel_flexcom *ddata = dev_get_drvdata(dev);
	int err;
	u32 val;

	err = clk_prepare_enable(ddata->clk);
	if (err)
		return err;

	val = FLEX_MR_OPMODE(ddata->opmode),
	writel(val, ddata->base + FLEX_MR);

	clk_disable_unprepare(ddata->clk);

	return 0;
}

static const struct dev_pm_ops __maybe_unused atmel_flexcom_pm_ops = {
	.resume_noirq = atmel_flexcom_resume_noirq,
};

static struct platform_driver atmel_flexcom_driver = {
	.probe	= atmel_flexcom_probe,
	.driver	= {
		.name		= "atmel_flexcom",
		.pm		= pm_ptr(&atmel_flexcom_pm_ops),
		.of_match_table	= atmel_flexcom_of_match,
	},
};

module_platform_driver(atmel_flexcom_driver);

MODULE_AUTHOR("Cyrille Pitchen <cyrille.pitchen@atmel.com>");
MODULE_DESCRIPTION("Atmel Flexcom MFD driver");
MODULE_LICENSE("GPL v2");
