/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2017, The freax Foundation. All rights reserved.
 */

#ifndef QCOM_PHY_QMP_QSERDES_TXRX_H_
#define QCOM_PHY_QMP_QSERDES_TXRX_H_

/* Only for QMP V2 PHY - TX registers */
#define QSERDES_TX_BIST_MODE_LANENO			0x000
#define QSERDES_TX_BIST_INVERT				0x004
#define QSERDES_TX_CLKBUF_ENABLE			0x008
#define QSERDES_TX_CMN_CONTROL_ONE			0x00c
#define QSERDES_TX_CMN_CONTROL_TWO			0x010
#define QSERDES_TX_CMN_CONTROL_THREE			0x014
#define QSERDES_TX_TX_EMP_POST1_LVL			0x018
#define QSERDES_TX_TX_POST2_EMPH			0x01c
#define QSERDES_TX_TX_BOOST_LVL_UP_DN			0x020
#define QSERDES_TX_HP_PD_ENABLES			0x024
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP		0x028
#define QSERDES_TX_TX_DRV_LVL				0x02c
#define QSERDES_TX_TX_DRV_LVL_OFFSET			0x030
#define QSERDES_TX_RESET_TSYNC_EN			0x034
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN		0x038
#define QSERDES_TX_TX_BAND				0x03c
#define QSERDES_TX_SLEW_CNTL				0x040
#define QSERDES_TX_INTERFACE_SELECT			0x044
#define QSERDES_TX_LPB_EN				0x048
#define QSERDES_TX_RES_CODE_LANE_TX			0x04c
#define QSERDES_TX_RES_CODE_LANE_RX			0x050
#define QSERDES_TX_RES_CODE_LANE_OFFSET			0x054
#define QSERDES_TX_PERL_LENGTH1				0x058
#define QSERDES_TX_PERL_LENGTH2				0x05c
#define QSERDES_TX_SERDES_BYP_EN_OUT			0x060
#define QSERDES_TX_DEBUG_BUS_SEL			0x064
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN	0x068
#define QSERDES_TX_TX_POL_INV				0x06c
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN		0x070
#define QSERDES_TX_BIST_PATTERN1			0x074
#define QSERDES_TX_BIST_PATTERN2			0x078
#define QSERDES_TX_BIST_PATTERN3			0x07c
#define QSERDES_TX_BIST_PATTERN4			0x080
#define QSERDES_TX_BIST_PATTERN5			0x084
#define QSERDES_TX_BIST_PATTERN6			0x088
#define QSERDES_TX_BIST_PATTERN7			0x08c
#define QSERDES_TX_BIST_PATTERN8			0x090
#define QSERDES_TX_LANE_MODE				0x094
#define QSERDES_TX_IDAC_CAL_LANE_MODE			0x098
#define QSERDES_TX_IDAC_CAL_LANE_MODE_CONFIGURATION	0x09c
#define QSERDES_TX_ATB_SEL1				0x0a0
#define QSERDES_TX_ATB_SEL2				0x0a4
#define QSERDES_TX_RCV_DETECT_LVL			0x0a8
#define QSERDES_TX_RCV_DETECT_LVL_2			0x0ac
#define QSERDES_TX_PRBS_SEED1				0x0b0
#define QSERDES_TX_PRBS_SEED2				0x0b4
#define QSERDES_TX_PRBS_SEED3				0x0b8
#define QSERDES_TX_PRBS_SEED4				0x0bc
#define QSERDES_TX_RESET_GEN				0x0c0
#define QSERDES_TX_RESET_GEN_MUXES			0x0c4
#define QSERDES_TX_TRAN_DRVR_EMP_EN			0x0c8
#define QSERDES_TX_TX_INTERFACE_MODE			0x0cc
#define QSERDES_TX_PWM_CTRL				0x0d0
#define QSERDES_TX_PWM_ENCODED_OR_DATA			0x0d4
#define QSERDES_TX_PWM_GEAR_1_DIVIDER_BAND2		0x0d8
#define QSERDES_TX_PWM_GEAR_2_DIVIDER_BAND2		0x0dc
#define QSERDES_TX_PWM_GEAR_3_DIVIDER_BAND2		0x0e0
#define QSERDES_TX_PWM_GEAR_4_DIVIDER_BAND2		0x0e4
#define QSERDES_TX_PWM_GEAR_1_DIVIDER_BAND0_1		0x0e8
#define QSERDES_TX_PWM_GEAR_2_DIVIDER_BAND0_1		0x0ec
#define QSERDES_TX_PWM_GEAR_3_DIVIDER_BAND0_1		0x0f0
#define QSERDES_TX_PWM_GEAR_4_DIVIDER_BAND0_1		0x0f4
#define QSERDES_TX_VMODE_CTRL1				0x0f8
#define QSERDES_TX_VMODE_CTRL2				0x0fc
#define QSERDES_TX_TX_ALOG_INTF_OBSV_CNTL		0x100
#define QSERDES_TX_BIST_STATUS				0x104
#define QSERDES_TX_BIST_ERROR_COUNT1			0x108
#define QSERDES_TX_BIST_ERROR_COUNT2			0x10c
#define QSERDES_TX_TX_ALOG_INTF_OBSV			0x110

/* Only for QMP V2 PHY - RX registers */
#define QSERDES_RX_UCDR_FO_GAIN_HALF			0x000
#define QSERDES_RX_UCDR_FO_GAIN_QUARTER			0x004
#define QSERDES_RX_UCDR_FO_GAIN_EIGHTH			0x008
#define QSERDES_RX_UCDR_FO_GAIN				0x00c
#define QSERDES_RX_UCDR_SO_GAIN_HALF			0x010
#define QSERDES_RX_UCDR_SO_GAIN_QUARTER			0x014
#define QSERDES_RX_UCDR_SO_GAIN_EIGHTH			0x018
#define QSERDES_RX_UCDR_SO_GAIN				0x01c
#define QSERDES_RX_UCDR_SVS_FO_GAIN_HALF		0x020
#define QSERDES_RX_UCDR_SVS_FO_GAIN_QUARTER		0x024
#define QSERDES_RX_UCDR_SVS_FO_GAIN_EIGHTH		0x028
#define QSERDES_RX_UCDR_SVS_FO_GAIN			0x02c
#define QSERDES_RX_UCDR_SVS_SO_GAIN_HALF		0x030
#define QSERDES_RX_UCDR_SVS_SO_GAIN_QUARTER		0x034
#define QSERDES_RX_UCDR_SVS_SO_GAIN_EIGHTH		0x038
#define QSERDES_RX_UCDR_SVS_SO_GAIN			0x03c
#define QSERDES_RX_UCDR_FASTLOCK_FO_GAIN		0x040
#define QSERDES_RX_UCDR_FD_GAIN				0x044
#define QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE	0x048
#define QSERDES_RX_UCDR_FO_TO_SO_DELAY			0x04c
#define QSERDES_RX_UCDR_FASTLOCK_COUNT_LOW		0x050
#define QSERDES_RX_UCDR_FASTLOCK_COUNT_HIGH		0x054
#define QSERDES_RX_UCDR_MODULATE			0x058
#define QSERDES_RX_UCDR_PI_CONTROLS			0x05c
#define QSERDES_RX_RBIST_CONTROL			0x060
#define QSERDES_RX_AUX_CONTROL				0x064
#define QSERDES_RX_AUX_DATA_TCOARSE			0x068
#define QSERDES_RX_AUX_DATA_TFINE_LSB			0x06c
#define QSERDES_RX_AUX_DATA_TFINE_MSB			0x070
#define QSERDES_RX_RCLK_AUXDATA_SEL			0x074
#define QSERDES_RX_AC_JTAG_ENABLE			0x078
#define QSERDES_RX_AC_JTAG_INITP			0x07c
#define QSERDES_RX_AC_JTAG_INITN			0x080
#define QSERDES_RX_AC_JTAG_LVL				0x084
#define QSERDES_RX_AC_JTAG_MODE				0x088
#define QSERDES_RX_AC_JTAG_RESET			0x08c
#define QSERDES_RX_RX_TERM_BW				0x090
#define QSERDES_RX_RX_RCVR_IQ_EN			0x094
#define QSERDES_RX_RX_IDAC_I_DC_OFFSETS			0x098
#define QSERDES_RX_RX_IDAC_IBAR_DC_OFFSETS		0x09c
#define QSERDES_RX_RX_IDAC_Q_DC_OFFSETS			0x0a0
#define QSERDES_RX_RX_IDAC_QBAR_DC_OFFSETS		0x0a4
#define QSERDES_RX_RX_IDAC_A_DC_OFFSETS			0x0a8
#define QSERDES_RX_RX_IDAC_ABAR_DC_OFFSETS		0x0ac
#define QSERDES_RX_RX_IDAC_EN				0x0b0
#define QSERDES_RX_RX_IDAC_ENABLES			0x0b4
#define QSERDES_RX_RX_IDAC_SIGN				0x0b8
#define QSERDES_RX_RX_HIGHZ_HIGHRATE			0x0bc
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET	0x0c0
#define QSERDES_RX_RX_EQ_GAIN1_LSB			0x0c4
#define QSERDES_RX_RX_EQ_GAIN1_MSB			0x0c8
#define QSERDES_RX_RX_EQ_GAIN2_LSB			0x0cc
#define QSERDES_RX_RX_EQ_GAIN2_MSB			0x0d0
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1		0x0d4
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2		0x0d8
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3		0x0dc
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4		0x0e0
#define QSERDES_RX_RX_IDAC_CAL_CONFIGURATION		0x0e4
#define QSERDES_RX_RX_IDAC_TSETTLE_LOW			0x0e8
#define QSERDES_RX_RX_IDAC_TSETTLE_HIGH			0x0ec
#define QSERDES_RX_RX_IDAC_ENDSAMP_LOW			0x0f0
#define QSERDES_RX_RX_IDAC_ENDSAMP_HIGH			0x0f4
#define QSERDES_RX_RX_IDAC_MIDPOINT_LOW			0x0f8
#define QSERDES_RX_RX_IDAC_MIDPOINT_HIGH		0x0fc
#define QSERDES_RX_RX_EQ_OFFSET_LSB			0x100
#define QSERDES_RX_RX_EQ_OFFSET_MSB			0x104
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1		0x108
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2		0x10c
#define QSERDES_RX_SIGDET_ENABLES			0x110
#define QSERDES_RX_SIGDET_CNTRL				0x114
#define QSERDES_RX_SIGDET_LVL				0x118
#define QSERDES_RX_SIGDET_DEGLITCH_CNTRL		0x11c
#define QSERDES_RX_RX_BAND				0x120
#define QSERDES_RX_CDR_FREEZE_UP_DN			0x124
#define QSERDES_RX_CDR_RESET_OVERRIDE			0x128
#define QSERDES_RX_RX_INTERFACE_MODE			0x12c
#define QSERDES_RX_JITTER_GEN_MODE			0x130
#define QSERDES_RX_BUJ_AMP				0x134
#define QSERDES_RX_SJ_AMP1				0x138
#define QSERDES_RX_SJ_AMP2				0x13c
#define QSERDES_RX_SJ_PER1				0x140
#define QSERDES_RX_SJ_PER2				0x144
#define QSERDES_RX_BUJ_STEP_FREQ1			0x148
#define QSERDES_RX_BUJ_STEP_FREQ2			0x14c
#define QSERDES_RX_PPM_OFFSET1				0x150
#define QSERDES_RX_PPM_OFFSET2				0x154
#define QSERDES_RX_SIGN_PPM_PERIOD1			0x158
#define QSERDES_RX_SIGN_PPM_PERIOD2			0x15c
#define QSERDES_RX_SSC_CTRL				0x160
#define QSERDES_RX_SSC_COUNT1				0x164
#define QSERDES_RX_SSC_COUNT2				0x168
#define QSERDES_RX_RX_ALOG_INTF_OBSV_CNTL		0x16c
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA		0x170
#define QSERDES_RX_RX_PWM_GEAR1_TIMEOUT_COUNT		0x174
#define QSERDES_RX_RX_PWM_GEAR2_TIMEOUT_COUNT		0x178
#define QSERDES_RX_RX_PWM_GEAR3_TIMEOUT_COUNT		0x17c
#define QSERDES_RX_RX_PWM_GEAR4_TIMEOUT_COUNT		0x180
#define QSERDES_RX_PI_CTRL1				0x184
#define QSERDES_RX_PI_CTRL2				0x188
#define QSERDES_RX_PI_QUAD				0x18c
#define QSERDES_RX_IDATA1				0x190
#define QSERDES_RX_IDATA2				0x194
#define QSERDES_RX_AUX_DATA1				0x198
#define QSERDES_RX_AUX_DATA2				0x19c
#define QSERDES_RX_AC_JTAG_OUTP				0x1a0
#define QSERDES_RX_AC_JTAG_OUTN				0x1a4
#define QSERDES_RX_RX_SIGDET				0x1a8
#define QSERDES_RX_RX_VDCOFF				0x1ac
#define QSERDES_RX_IDAC_CAL_ON				0x1b0
#define QSERDES_RX_IDAC_STATUS_I			0x1b4
#define QSERDES_RX_IDAC_STATUS_IBAR			0x1b8
#define QSERDES_RX_IDAC_STATUS_Q			0x1bc
#define QSERDES_RX_IDAC_STATUS_QBAR			0x1c0
#define QSERDES_RX_IDAC_STATUS_A			0x1c4
#define QSERDES_RX_IDAC_STATUS_ABAR			0x1c8
#define QSERDES_RX_CALST_STATUS_I			0x1cc
#define QSERDES_RX_CALST_STATUS_Q			0x1d0
#define QSERDES_RX_CALST_STATUS_A			0x1d4
#define QSERDES_RX_RX_ALOG_INTF_OBSV			0x1d8
#define QSERDES_RX_READ_EQCODE				0x1dc
#define QSERDES_RX_READ_OFFSETCODE			0x1e0
#define QSERDES_RX_IA_ERROR_COUNTER_LOW			0x1e4
#define QSERDES_RX_IA_ERROR_COUNTER_HIGH		0x1e8

#endif
