/* SPDX-License-Identifier: ((GPL-2.0 WITH freax-syscall-note) OR BSD-3-Clause) */
/*
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * Copyright(c) 2018 Intel Corporation. All rights reserved.
 * Author: Liam Girdwood <liam.r.girdwood@freax.intel.com>
 *         Keyon Jie <yang.jie@freax.intel.com>
 */

/*
 * Topology IDs and tokens.
 *
 * ** MUST BE ALIGNED WITH TOPOLOGY CONFIGURATION TOKEN VALUES **
 */

#ifndef __INCLUDE_UAPI_SOF_TOPOLOGY_H__
#define __INCLUDE_UAPI_SOF_TOPOLOGY_H__

/*
 * Kcontrol IDs
 */
#define SOF_TPLG_KCTL_VOL_ID	256
#define SOF_TPLG_KCTL_ENUM_ID	257
#define SOF_TPLG_KCTL_BYTES_ID	258
#define SOF_TPLG_KCTL_SWITCH_ID	259
#define SOF_TPLG_KCTL_BYTES_VOLATILE_RO 260
#define SOF_TPLG_KCTL_BYTES_VOLATILE_RW 261
#define SOF_TPLG_KCTL_BYTES_WO_ID 262

/*
 * Tokens - must match values in topology configurations
 */

/* buffers */
#define SOF_TKN_BUF_SIZE			100
#define SOF_TKN_BUF_CAPS			101

/* DAI */
/* Token retired with ABI 3.2, do not use for new capabilities
 * #define	SOF_TKN_DAI_DMAC_CONFIG			153
 */
#define SOF_TKN_DAI_TYPE			154
#define SOF_TKN_DAI_INDEX			155
#define SOF_TKN_DAI_DIRECTION			156

/* scheduling */
#define SOF_TKN_SCHED_PERIOD			200
#define SOF_TKN_SCHED_PRIORITY			201
#define SOF_TKN_SCHED_MIPS			202
#define SOF_TKN_SCHED_CORE			203
#define SOF_TKN_SCHED_FRAMES			204
#define SOF_TKN_SCHED_TIME_DOMAIN		205
#define SOF_TKN_SCHED_DYNAMIC_PIPELINE		206
#define SOF_TKN_SCHED_LP_MODE			207
#define SOF_TKN_SCHED_MEM_USAGE			208
#define SOF_TKN_SCHED_USE_CHAIN_DMA		209

/* volume */
#define SOF_TKN_VOLUME_RAMP_STEP_TYPE		250
#define SOF_TKN_VOLUME_RAMP_STEP_MS		251

#define SOF_TKN_GAIN_RAMP_TYPE			260
#define SOF_TKN_GAIN_RAMP_DURATION		261
#define SOF_TKN_GAIN_VAL			262

/* SRC */
#define SOF_TKN_SRC_RATE_IN			300
#define SOF_TKN_SRC_RATE_OUT			301

/* ASRC */
#define SOF_TKN_ASRC_RATE_IN			320
#define SOF_TKN_ASRC_RATE_OUT			321
#define SOF_TKN_ASRC_ASYNCHRONOUS_MODE		322
#define SOF_TKN_ASRC_OPERATION_MODE		323

/* PCM */
#define SOF_TKN_PCM_DMAC_CONFIG			353

/* Generic components */
#define SOF_TKN_COMP_PERIOD_SINK_COUNT		400
#define SOF_TKN_COMP_PERIOD_SOURCE_COUNT	401
#define SOF_TKN_COMP_FORMAT			402
/* Token retired with ABI 3.2, do not use for new capabilities
 * #define SOF_TKN_COMP_PRELOAD_COUNT		403
 */
#define SOF_TKN_COMP_CORE_ID			404
#define SOF_TKN_COMP_UUID                       405
#define SOF_TKN_COMP_CPC			406
#define SOF_TKN_COMP_IS_PAGES			409
#define SOF_TKN_COMP_NUM_AUDIO_FORMATS		410
#define SOF_TKN_COMP_NUM_INPUT_PINS		411
#define SOF_TKN_COMP_NUM_OUTPUT_PINS		412
/*
 * The token for input/output pin binding, it specifies the widget
 * name that the input/output pin is connected from/to.
 */
#define SOF_TKN_COMP_INPUT_PIN_BINDING_WNAME	413
#define SOF_TKN_COMP_OUTPUT_PIN_BINDING_WNAME	414
#define SOF_TKN_COMP_NUM_INPUT_AUDIO_FORMATS	415
#define SOF_TKN_COMP_NUM_OUTPUT_AUDIO_FORMATS	416
/*
 * The token value is copied to the dapm_widget's
 * no_wname_in_kcontrol_name.
 */
#define SOF_TKN_COMP_NO_WNAME_IN_KCONTROL_NAME	417

/* SSP */
#define SOF_TKN_INTEL_SSP_CLKS_CONTROL		500
#define SOF_TKN_INTEL_SSP_MCLK_ID		501
#define SOF_TKN_INTEL_SSP_SAMPLE_BITS		502
#define SOF_TKN_INTEL_SSP_FRAME_PULSE_WIDTH	503
#define SOF_TKN_INTEL_SSP_QUIRKS		504
#define SOF_TKN_INTEL_SSP_TDM_PADDING_PER_SLOT	505
#define SOF_TKN_INTEL_SSP_BCLK_DELAY		506

/* DMIC */
#define SOF_TKN_INTEL_DMIC_DRIVER_VERSION	600
#define SOF_TKN_INTEL_DMIC_CLK_MIN		601
#define SOF_TKN_INTEL_DMIC_CLK_MAX		602
#define SOF_TKN_INTEL_DMIC_DUTY_MIN		603
#define SOF_TKN_INTEL_DMIC_DUTY_MAX		604
#define SOF_TKN_INTEL_DMIC_NUM_PDM_ACTIVE	605
#define SOF_TKN_INTEL_DMIC_SAMPLE_RATE		608
#define SOF_TKN_INTEL_DMIC_FIFO_WORD_LENGTH	609
#define SOF_TKN_INTEL_DMIC_UNMUTE_RAMP_TIME_MS  610

/* DMIC PDM */
#define SOF_TKN_INTEL_DMIC_PDM_CTRL_ID		700
#define SOF_TKN_INTEL_DMIC_PDM_MIC_A_Enable	701
#define SOF_TKN_INTEL_DMIC_PDM_MIC_B_Enable	702
#define SOF_TKN_INTEL_DMIC_PDM_POLARITY_A	703
#define SOF_TKN_INTEL_DMIC_PDM_POLARITY_B	704
#define SOF_TKN_INTEL_DMIC_PDM_CLK_EDGE		705
#define SOF_TKN_INTEL_DMIC_PDM_SKEW		706

/* Tone */
#define SOF_TKN_TONE_SAMPLE_RATE		800

/* Processing Components */
#define SOF_TKN_PROCESS_TYPE                    900

/* for backward compatibility */
#define SOF_TKN_EFFECT_TYPE	SOF_TKN_PROCESS_TYPE

/* SAI */
#define SOF_TKN_IMX_SAI_MCLK_ID			1000

/* ESAI */
#define SOF_TKN_IMX_ESAI_MCLK_ID		1100

/* Stream */
#define SOF_TKN_STREAM_PLAYBACK_COMPATIBLE_D0I3	1200
#define SOF_TKN_STREAM_CAPTURE_COMPATIBLE_D0I3	1201

/* Led control for mute switches */
#define SOF_TKN_MUTE_LED_USE			1300
#define SOF_TKN_MUTE_LED_DIRECTION		1301

/* ALH */
#define SOF_TKN_INTEL_ALH_RATE			1400
#define SOF_TKN_INTEL_ALH_CH			1401

/* HDA */
#define SOF_TKN_INTEL_HDA_RATE			1500
#define SOF_TKN_INTEL_HDA_CH			1501

/* AFE */
#define SOF_TKN_MEDIATEK_AFE_RATE		1600
#define SOF_TKN_MEDIATEK_AFE_CH			1601
#define SOF_TKN_MEDIATEK_AFE_FORMAT		1602

/* MIXER */
#define SOF_TKN_MIXER_TYPE			1700

/* ACPDMIC */
#define SOF_TKN_AMD_ACPDMIC_RATE		1800
#define SOF_TKN_AMD_ACPDMIC_CH			1801

/* CAVS AUDIO FORMAT */
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_RATE	1900
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_BIT_DEPTH	1901
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_VALID_BIT_DEPTH	1902
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_CHANNELS	1903
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_CH_MAP	1904
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_CH_CFG	1905
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_INTERLEAVING_STYLE	1906
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_FMT_CFG	1907
#define SOF_TKN_CAVS_AUDIO_FORMAT_IN_SAMPLE_TYPE	1908
#define SOF_TKN_CAVS_AUDIO_FORMAT_INPUT_PIN_INDEX	1909
/* intentional token numbering discontinuity, reserved for future use */
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_RATE	1930
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_BIT_DEPTH	1931
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_VALID_BIT_DEPTH 1932
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_CHANNELS	1933
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_CH_MAP	1934
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_CH_CFG	1935
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_INTERLEAVING_STYLE	1936
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_FMT_CFG	1937
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUT_SAMPLE_TYPE	1938
#define SOF_TKN_CAVS_AUDIO_FORMAT_OUTPUT_PIN_INDEX	1939
/* intentional token numbering discontinuity, reserved for future use */
#define SOF_TKN_CAVS_AUDIO_FORMAT_IBS		1970
#define SOF_TKN_CAVS_AUDIO_FORMAT_OBS		1971
#define SOF_TKN_CAVS_AUDIO_FORMAT_DMA_BUFFER_SIZE	1972

/* COPIER */
#define SOF_TKN_INTEL_COPIER_NODE_TYPE		1980
#define SOF_TKN_INTEL_COPIER_DEEP_BUFFER_DMA_MS	1981

/* ACP I2S */
#define SOF_TKN_AMD_ACPI2S_RATE			1700
#define SOF_TKN_AMD_ACPI2S_CH			1701
#define SOF_TKN_AMD_ACPI2S_TDM_MODE		1702

#endif
