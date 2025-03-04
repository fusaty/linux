/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * OMAP44xx CM2 instance offset macros
 *
 * Copyright (C) 2009-2011 Texas Instruments, Inc.
 * Copyright (C) 2009-2010 Nokia Corporation
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
 *
 * XXX This file needs to be updated to align on one of "OMAP4", "OMAP44XX",
 *     or "OMAP4430".
 */

#ifndef __ARCH_ARM_MACH_OMAP2_CM2_44XX_H
#define __ARCH_ARM_MACH_OMAP2_CM2_44XX_H

/* CM2 base address */
#define OMAP4430_CM2_BASE		0x4a008000

#define OMAP44XX_CM2_REGADDR(inst, reg)				\
	OMAP2_L4_IO_ADDRESS(OMAP4430_CM2_BASE + (inst) + (reg))

/* CM2 instances */
#define OMAP4430_CM2_OCP_SOCKET_INST	0x0000
#define OMAP4430_CM2_CKGEN_INST		0x0100
#define OMAP4430_CM2_ALWAYS_ON_INST	0x0600
#define OMAP4430_CM2_CORE_INST		0x0700
#define OMAP4430_CM2_IVAHD_INST		0x0f00
#define OMAP4430_CM2_CAM_INST		0x1000
#define OMAP4430_CM2_DSS_INST		0x1100
#define OMAP4430_CM2_GFX_INST		0x1200
#define OMAP4430_CM2_L3INIT_INST	0x1300
#define OMAP4430_CM2_L4PER_INST		0x1400
#define OMAP4430_CM2_CEFUSE_INST	0x1600

/* CM2 clockdomain register offsets (from instance start) */
#define OMAP4430_CM2_ALWAYS_ON_ALWON_CDOFFS	0x0000
#define OMAP4430_CM2_CORE_L3_1_CDOFFS		0x0000
#define OMAP4430_CM2_CORE_L3_2_CDOFFS		0x0100
#define OMAP4430_CM2_CORE_DUCATI_CDOFFS		0x0200
#define OMAP4430_CM2_CORE_SDMA_CDOFFS		0x0300
#define OMAP4430_CM2_CORE_MEMIF_CDOFFS		0x0400
#define OMAP4430_CM2_CORE_D2D_CDOFFS		0x0500
#define OMAP4430_CM2_CORE_L4CFG_CDOFFS		0x0600
#define OMAP4430_CM2_CORE_L3INSTR_CDOFFS	0x0700
#define OMAP4430_CM2_IVAHD_IVAHD_CDOFFS		0x0000
#define OMAP4430_CM2_CAM_CAM_CDOFFS		0x0000
#define OMAP4430_CM2_DSS_DSS_CDOFFS		0x0000
#define OMAP4430_CM2_GFX_GFX_CDOFFS		0x0000
#define OMAP4430_CM2_L3INIT_L3INIT_CDOFFS	0x0000
#define OMAP4430_CM2_L4PER_L4PER_CDOFFS		0x0000
#define OMAP4430_CM2_L4PER_L4SEC_CDOFFS		0x0180
#define OMAP4430_CM2_CEFUSE_CEFUSE_CDOFFS	0x0000

#endif
