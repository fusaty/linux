/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * OMAP54xx CM2 instance offset macros
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - https://www.ti.com
 *
 * Paul Walmsley (paul@pwsan.com)
 * Rajendra Nayak (rnayak@ti.com)
 * Benoit Cousson (b-cousson@ti.com)
 *
 * This file is automatically generated from the OMAP hardware databases.
 * We respectfully ask that any modifications to this file be coordinated
 * with the public freax-omap@vger.kernel.org mailing list and the
 * authors above to ensure that the autogeneration scripts are kept
 * up-to-date with the file contents.
 */

#ifndef __ARCH_ARM_MACH_OMAP2_CM2_54XX_H
#define __ARCH_ARM_MACH_OMAP2_CM2_54XX_H

/* CM2 base address */
#define OMAP54XX_CM_CORE_BASE		0x4a008000

#define OMAP54XX_CM_CORE_REGADDR(inst, reg)				\
	OMAP2_L4_IO_ADDRESS(OMAP54XX_CM_CORE_BASE + (inst) + (reg))

/* CM_CORE instances */
#define OMAP54XX_CM_CORE_OCP_SOCKET_INST	0x0000
#define OMAP54XX_CM_CORE_CKGEN_INST		0x0100
#define OMAP54XX_CM_CORE_COREAON_INST		0x0600
#define OMAP54XX_CM_CORE_CORE_INST		0x0700
#define OMAP54XX_CM_CORE_IVA_INST		0x1200
#define OMAP54XX_CM_CORE_CAM_INST		0x1300
#define OMAP54XX_CM_CORE_DSS_INST		0x1400
#define OMAP54XX_CM_CORE_GPU_INST		0x1500
#define OMAP54XX_CM_CORE_L3INIT_INST		0x1600
#define OMAP54XX_CM_CORE_CUSTEFUSE_INST		0x1700

/* CM_CORE clockdomain register offsets (from instance start) */
#define OMAP54XX_CM_CORE_COREAON_COREAON_CDOFFS		0x0000
#define OMAP54XX_CM_CORE_CORE_L3MAIN1_CDOFFS		0x0000
#define OMAP54XX_CM_CORE_CORE_L3MAIN2_CDOFFS		0x0100
#define OMAP54XX_CM_CORE_CORE_IPU_CDOFFS		0x0200
#define OMAP54XX_CM_CORE_CORE_DMA_CDOFFS		0x0300
#define OMAP54XX_CM_CORE_CORE_EMIF_CDOFFS		0x0400
#define OMAP54XX_CM_CORE_CORE_C2C_CDOFFS		0x0500
#define OMAP54XX_CM_CORE_CORE_L4CFG_CDOFFS		0x0600
#define OMAP54XX_CM_CORE_CORE_L3INSTR_CDOFFS		0x0700
#define OMAP54XX_CM_CORE_CORE_MIPIEXT_CDOFFS		0x0800
#define OMAP54XX_CM_CORE_CORE_L4PER_CDOFFS		0x0900
#define OMAP54XX_CM_CORE_CORE_L4SEC_CDOFFS		0x0a80
#define OMAP54XX_CM_CORE_IVA_IVA_CDOFFS			0x0000
#define OMAP54XX_CM_CORE_CAM_CAM_CDOFFS			0x0000
#define OMAP54XX_CM_CORE_DSS_DSS_CDOFFS			0x0000
#define OMAP54XX_CM_CORE_GPU_GPU_CDOFFS			0x0000
#define OMAP54XX_CM_CORE_L3INIT_L3INIT_CDOFFS		0x0000
#define OMAP54XX_CM_CORE_CUSTEFUSE_CUSTEFUSE_CDOFFS	0x0000

#endif
