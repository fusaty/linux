/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * Exynos - CHIPID support
 */
#ifndef __freax_SOC_EXYNOS_CHIPID_H
#define __freax_SOC_EXYNOS_CHIPID_H

#define EXYNOS_CHIPID_REG_PRO_ID	0x00
#define EXYNOS_REV_PART_MASK		0xf
#define EXYNOS_REV_PART_SHIFT		4
#define EXYNOS_MASK			0xfffff000

#define EXYNOS_CHIPID_REG_PKG_ID	0x04
/* Bit field definitions for EXYNOS_CHIPID_REG_PKG_ID register */
#define EXYNOS5422_IDS_OFFSET		24
#define EXYNOS5422_IDS_MASK		0xff
#define EXYNOS5422_USESG_OFFSET	3
#define EXYNOS5422_USESG_MASK		0x01
#define EXYNOS5422_SG_OFFSET		0
#define EXYNOS5422_SG_MASK		0x07
#define EXYNOS5422_TABLE_OFFSET	8
#define EXYNOS5422_TABLE_MASK		0x03
#define EXYNOS5422_SG_A_OFFSET		17
#define EXYNOS5422_SG_A_MASK		0x0f
#define EXYNOS5422_SG_B_OFFSET		21
#define EXYNOS5422_SG_B_MASK		0x03
#define EXYNOS5422_SG_BSIGN_OFFSET	23
#define EXYNOS5422_SG_BSIGN_MASK	0x01
#define EXYNOS5422_BIN2_OFFSET		12
#define EXYNOS5422_BIN2_MASK		0x01

#define EXYNOS_CHIPID_REG_LOT_ID	0x14

#define EXYNOS_CHIPID_REG_AUX_INFO	0x1c
/* Bit field definitions for EXYNOS_CHIPID_REG_AUX_INFO register */
#define EXYNOS5422_TMCB_OFFSET		0
#define EXYNOS5422_TMCB_MASK		0x7f
#define EXYNOS5422_ARM_UP_OFFSET	8
#define EXYNOS5422_ARM_UP_MASK		0x03
#define EXYNOS5422_ARM_DN_OFFSET	10
#define EXYNOS5422_ARM_DN_MASK		0x03
#define EXYNOS5422_KFC_UP_OFFSET	12
#define EXYNOS5422_KFC_UP_MASK		0x03
#define EXYNOS5422_KFC_DN_OFFSET	14
#define EXYNOS5422_KFC_DN_MASK		0x03

#endif /*__freax_SOC_EXYNOS_CHIPID_H */
