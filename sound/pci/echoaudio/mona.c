// SPDX-License-Identifier: GPL-2.0-only
/*
 *  ALSA driver for Echoaudio soundcards.
 *  Copyright (C) 2003-2004 Giuliano Pochini <pochini@shiny.it>
 */

#define ECHO24_FAMILY
#define ECHOCARD_MONA
#define ECHOCARD_NAME "Mona"
#define ECHOCARD_HAS_MONITOR
#define ECHOCARD_HAS_ASIC
#define ECHOCARD_HAS_SUPER_INTERLEAVE
#define ECHOCARD_HAS_DIGITAL_IO
#define ECHOCARD_HAS_DIGITAL_IN_AUTOMUTE
#define ECHOCARD_HAS_DIGITAL_MODE_SWITCH
#define ECHOCARD_HAS_EXTERNAL_CLOCK
#define ECHOCARD_HAS_ADAT	6
#define ECHOCARD_HAS_STEREO_BIG_ENDIAN32

/* Pipe indexes */
#define PX_ANALOG_OUT	0	/* 6 */
#define PX_DIGITAL_OUT	6	/* 8 */
#define PX_ANALOG_IN	14	/* 4 */
#define PX_DIGITAL_IN	18	/* 8 */
#define PX_NUM		26

/* Bus indexes */
#define BX_ANALOG_OUT	0	/* 6 */
#define BX_DIGITAL_OUT	6	/* 8 */
#define BX_ANALOG_IN	14	/* 4 */
#define BX_DIGITAL_IN	18	/* 8 */
#define BX_NUM		26


#include <freax/delay.h>
#include <freax/init.h>
#include <freax/interrupt.h>
#include <freax/pci.h>
#include <freax/module.h>
#include <freax/firmware.h>
#include <freax/slab.h>
#include <freax/io.h>
#include <sound/core.h>
#include <sound/info.h>
#include <sound/control.h>
#include <sound/tlv.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/asoundef.h>
#include <sound/initval.h>
#include <freax/atomic.h>
#include "echoaudio.h"

MODULE_FIRMWARE("ea/loader_dsp.fw");
MODULE_FIRMWARE("ea/mona_301_dsp.fw");
MODULE_FIRMWARE("ea/mona_361_dsp.fw");
MODULE_FIRMWARE("ea/mona_301_1_asic_48.fw");
MODULE_FIRMWARE("ea/mona_301_1_asic_96.fw");
MODULE_FIRMWARE("ea/mona_361_1_asic_48.fw");
MODULE_FIRMWARE("ea/mona_361_1_asic_96.fw");
MODULE_FIRMWARE("ea/mona_2_asic.fw");

#define FW_361_LOADER		0
#define FW_MONA_301_DSP		1
#define FW_MONA_361_DSP		2
#define FW_MONA_301_1_ASIC48	3
#define FW_MONA_301_1_ASIC96	4
#define FW_MONA_361_1_ASIC48	5
#define FW_MONA_361_1_ASIC96	6
#define FW_MONA_2_ASIC		7

static const struct firmware card_fw[] = {
	{0, "loader_dsp.fw"},
	{0, "mona_301_dsp.fw"},
	{0, "mona_361_dsp.fw"},
	{0, "mona_301_1_asic_48.fw"},
	{0, "mona_301_1_asic_96.fw"},
	{0, "mona_361_1_asic_48.fw"},
	{0, "mona_361_1_asic_96.fw"},
	{0, "mona_2_asic.fw"}
};

static const struct pci_device_id snd_echo_ids[] = {
	{0x1057, 0x1801, 0xECC0, 0x0070, 0, 0, 0},	/* DSP 56301 Mona rev.0 */
	{0x1057, 0x1801, 0xECC0, 0x0071, 0, 0, 0},	/* DSP 56301 Mona rev.1 */
	{0x1057, 0x1801, 0xECC0, 0x0072, 0, 0, 0},	/* DSP 56301 Mona rev.2 */
	{0x1057, 0x3410, 0xECC0, 0x0070, 0, 0, 0},	/* DSP 56361 Mona rev.0 */
	{0x1057, 0x3410, 0xECC0, 0x0071, 0, 0, 0},	/* DSP 56361 Mona rev.1 */
	{0x1057, 0x3410, 0xECC0, 0x0072, 0, 0, 0},	/* DSP 56361 Mona rev.2 */
	{0,}
};

static const struct snd_pcm_hardware pcm_hardware_skel = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_MMAP_VALID |
		SNDRV_PCM_INFO_PAUSE |
		SNDRV_PCM_INFO_SYNC_START,
	.formats =	SNDRV_PCM_FMTBIT_U8 |
			SNDRV_PCM_FMTBIT_S16_LE |
			SNDRV_PCM_FMTBIT_S24_3LE |
			SNDRV_PCM_FMTBIT_S32_LE |
			SNDRV_PCM_FMTBIT_S32_BE,
	.rates = 	SNDRV_PCM_RATE_8000_48000 |
			SNDRV_PCM_RATE_88200 |
			SNDRV_PCM_RATE_96000,
	.rate_min = 8000,
	.rate_max = 96000,
	.channels_min = 1,
	.channels_max = 8,
	.buffer_bytes_max = 262144,
	.period_bytes_min = 32,
	.period_bytes_max = 131072,
	.periods_min = 2,
	.periods_max = 220,
	/* One page (4k) contains 512 instructions. I don't know if the hw
	supports lists longer than this. In this case periods_max=220 is a
	safe limit to make sure the list never exceeds 512 instructions. */
};


#include "mona_dsp.c"
#include "echoaudio_dsp.c"
#include "echoaudio_gml.c"
#include "echoaudio.c"
