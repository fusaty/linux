/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * rt1017-sdca-sdw.h -- RT1017 SDCA ALSA SoC audio driver header
 *
 * Copyright(c) 2023 Realtek Semiconductor Corp.
 */

#ifndef __RT1017_SDW_H__
#define __RT1017_SDW_H__

#include <freax/regmap.h>
#include <freax/soundwire/sdw.h>
#include <freax/soundwire/sdw_type.h>
#include <freax/soundwire/sdw_registers.h>
#include <sound/soc.h>

/* RT1017 SDCA Control - function number */
#define FUNC_NUM_SMART_AMP 0x04

/* RT1017 SDCA entity */
#define RT1017_SDCA_ENT_PDE23 0x31
#define RT1017_SDCA_ENT_PDE22 0x33
#define RT1017_SDCA_ENT_CS21 0x21
#define RT1017_SDCA_ENT_SAPU29 0x29
#define RT1017_SDCA_ENT_XU22 0x22
#define RT1017_SDCA_ENT_FU 0x03
#define RT1017_SDCA_ENT_UDMPU21 0x02

/* RT1017 SDCA control */
#define RT1017_SDCA_CTL_FS_INDEX 0x10
#define RT1017_SDCA_CTL_REQ_POWER_STATE 0x01
#define RT1017_SDCA_CTL_PROT_STAT 0x11
#define RT1017_SDCA_CTL_BYPASS 0x01
#define RT1017_SDCA_CTL_FU_MUTE 0x01
#define RT1017_SDCA_CTL_FU_VOLUME 0x02
#define RT1017_SDCA_CTL_UDMPU_CLUSTER 0x10


#define RT1017_CLASSD_INT_1		0xd300
#define RT1017_PWM_TRIM_1		0xd370


#define RT1017_PWM_FREQ_CTL_SRC_SEL_MASK	(0x3 << 2)
#define RT1017_PWM_FREQ_CTL_SRC_SEL_EFUSE	(0x2 << 2)
#define RT1017_PWM_FREQ_CTL_SRC_SEL_REG		(0x0 << 2)

enum {
	RT1017_SDCA_RATE_44100HZ = 0x8,
	RT1017_SDCA_RATE_48000HZ = 0x9,
	RT1017_SDCA_RATE_96000HZ = 0xb,
	RT1017_SDCA_RATE_192000HZ = 0xd,
};

struct rt1017_sdca_priv {
	struct snd_soc_component *component;
	struct regmap *regmap;
	struct sdw_slave *sdw_slave;
	struct sdw_bus_params params;
	bool hw_init;
	bool first_hw_init;
};

static const struct reg_default rt1017_sdca_reg_defaults[] = {
	{ 0x3206, 0x00 },
	{ 0xc001, 0x43 },
	{ 0xc030, 0x54 },
	{ 0xc104, 0x8a },
	{ 0xc10b, 0x2f },
	{ 0xc10c, 0x2f },
	{ 0xc110, 0x49 },
	{ 0xc112, 0x10 },
	{ 0xc300, 0xff },
	{ 0xc301, 0xdd },
	{ 0xc318, 0x40 },
	{ 0xc325, 0x00 },
	{ 0xc326, 0x00 },
	{ 0xc327, 0x00 },
	{ 0xc328, 0x02 },
	{ 0xc331, 0xb2 },
	{ 0xc340, 0x02 },
	{ 0xc350, 0x21 },
	{ 0xc500, 0x00 },
	{ 0xc502, 0x00 },
	{ 0xc504, 0x3f },
	{ 0xc507, 0x1f },
	{ 0xc509, 0x1f },
	{ 0xc510, 0x40 },
	{ 0xc512, 0x00 },
	{ 0xc518, 0x02 },
	{ 0xc51b, 0x7f },
	{ 0xc51d, 0x0f },
	{ 0xc520, 0x00 },
	{ 0xc540, 0x80 },
	{ 0xc541, 0x00 },
	{ 0xc542, 0x0a },
	{ 0xc550, 0x80 },
	{ 0xc551, 0x0f },
	{ 0xc552, 0xff },
	{ 0xc600, 0x10 },
	{ 0xc602, 0x83 },
	{ 0xc612, 0x40 },
	{ 0xc622, 0x40 },
	{ 0xc632, 0x40 },
	{ 0xc642, 0x40 },
	{ 0xc651, 0x00 },
	{ 0xca00, 0xc1 },
	{ 0xca09, 0x00 },
	{ 0xca0a, 0x51 },
	{ 0xca0b, 0xeb },
	{ 0xca0c, 0x85 },
	{ 0xca0e, 0x00 },
	{ 0xca0f, 0x10 },
	{ 0xca10, 0x62 },
	{ 0xca11, 0x4d },
	{ 0xca16, 0x0f },
	{ 0xca17, 0x00 },
	{ 0xcb00, 0x10 },
	{ 0xcc00, 0x10 },
	{ 0xcc02, 0x0b },
	{ 0xd017, 0x09 },
	{ 0xd01a, 0x00 },
	{ 0xd01b, 0x00 },
	{ 0xd01c, 0x00 },
	{ 0xd101, 0xa0 },
	{ 0xd20c, 0x14 },
	{ 0xd300, 0x0f },
	{ 0xd370, 0x18 },
	{ 0xd500, 0x00 },
	{ 0xd545, 0x0b },
	{ 0xd546, 0xf9 },
	{ 0xd547, 0xb2 },
	{ 0xd548, 0xa9 },
	{ 0xd5a5, 0x00 },
	{ 0xd5a6, 0x00 },
	{ 0xd5a7, 0x00 },
	{ 0xd5a8, 0x00 },
	{ 0xd5aa, 0x00 },
	{ 0xd5ab, 0x00 },
	{ 0xd5ac, 0x00 },
	{ 0xd5ad, 0x00 },
	{ 0xda04, 0x03 },
	{ 0xda05, 0x33 },
	{ 0xda06, 0x33 },
	{ 0xda07, 0x33 },
	{ 0xda09, 0x5d },
	{ 0xda0a, 0xc0 },
	{ 0xda0c, 0x00 },
	{ 0xda0d, 0x01 },
	{ 0xda0e, 0x5d },
	{ 0xda0f, 0x86 },
	{ 0xda11, 0x20 },
	{ 0xda12, 0x00 },
	{ 0xda13, 0x00 },
	{ 0xda14, 0x00 },
	{ 0xda16, 0x7f },
	{ 0xda17, 0xff },
	{ 0xda18, 0xff },
	{ 0xda19, 0xff },
	{ 0xdab6, 0x00 },
	{ 0xdab7, 0x01 },
	{ 0xdab8, 0x00 },
	{ 0xdab9, 0x01 },
	{ 0xdaba, 0x00 },
	{ 0xdabb, 0x01 },
	{ 0xdb09, 0x0f },
	{ 0xdb0a, 0xff },
	{ 0xdb14, 0x00 },

	{ SDW_SDCA_CTL(FUNC_NUM_SMART_AMP, RT1017_SDCA_ENT_UDMPU21,
			RT1017_SDCA_CTL_UDMPU_CLUSTER, 0), 0x00 },
	{ SDW_SDCA_CTL(FUNC_NUM_SMART_AMP, RT1017_SDCA_ENT_FU,
			RT1017_SDCA_CTL_FU_MUTE, 0x01), 0x01 },
	{ SDW_SDCA_CTL(FUNC_NUM_SMART_AMP, RT1017_SDCA_ENT_XU22,
			RT1017_SDCA_CTL_BYPASS, 0), 0x01 },
	{ SDW_SDCA_CTL(FUNC_NUM_SMART_AMP, RT1017_SDCA_ENT_CS21,
			RT1017_SDCA_CTL_FS_INDEX, 0), 0x09 },
	{ SDW_SDCA_CTL(FUNC_NUM_SMART_AMP, RT1017_SDCA_ENT_PDE23,
			RT1017_SDCA_CTL_REQ_POWER_STATE, 0), 0x03 },
	{ SDW_SDCA_CTL(FUNC_NUM_SMART_AMP, RT1017_SDCA_ENT_PDE22,
			RT1017_SDCA_CTL_REQ_POWER_STATE, 0), 0x03 },
};

#endif /* __RT1017_SDW_H__ */
