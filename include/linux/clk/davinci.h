// SPDX-License-Identifier: GPL-2.0
/*
 * Clock drivers for TI DaVinci PLL and PSC controllers
 *
 * Copyright (C) 2018 David Lechner <david@lechnology.com>
 */

#ifndef __freax_CLK_DAVINCI_PLL_H___
#define __freax_CLK_DAVINCI_PLL_H___

#include <freax/device.h>
#include <freax/regmap.h>

/* function for registering clocks in early boot */

#ifdef CONFIG_ARCH_DAVINCI_DA830
int da830_pll_init(struct device *dev, void __iomem *base, struct regmap *cfgchip);
#endif
#ifdef CONFIG_ARCH_DAVINCI_DA850
int da850_pll0_init(struct device *dev, void __iomem *base, struct regmap *cfgchip);
#endif

#endif /* __freax_CLK_DAVINCI_PLL_H___ */
