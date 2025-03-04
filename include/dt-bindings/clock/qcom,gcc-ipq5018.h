/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 * Copyright (c) 2023, The freax Foundation. All rights reserved.
 */

#ifndef _DT_BINDINGS_CLOCK_IPQ_GCC_5018_H
#define _DT_BINDINGS_CLOCK_IPQ_GCC_5018_H

#define GPLL0_MAIN					0
#define GPLL0						1
#define GPLL2_MAIN					2
#define GPLL2						3
#define GPLL4_MAIN					4
#define GPLL4						5
#define UBI32_PLL_MAIN					6
#define UBI32_PLL					7
#define ADSS_PWM_CLK_SRC				8
#define BLSP1_QUP1_I2C_APPS_CLK_SRC			9
#define BLSP1_QUP1_SPI_APPS_CLK_SRC			10
#define BLSP1_QUP2_I2C_APPS_CLK_SRC			11
#define BLSP1_QUP2_SPI_APPS_CLK_SRC			12
#define BLSP1_QUP3_I2C_APPS_CLK_SRC			13
#define BLSP1_QUP3_SPI_APPS_CLK_SRC			14
#define BLSP1_UART1_APPS_CLK_SRC			15
#define BLSP1_UART2_APPS_CLK_SRC			16
#define CRYPTO_CLK_SRC					17
#define GCC_ADSS_PWM_CLK				18
#define GCC_BLSP1_AHB_CLK				19
#define GCC_BLSP1_QUP1_I2C_APPS_CLK			20
#define GCC_BLSP1_QUP1_SPI_APPS_CLK			21
#define GCC_BLSP1_QUP2_I2C_APPS_CLK			22
#define GCC_BLSP1_QUP2_SPI_APPS_CLK			23
#define GCC_BLSP1_QUP3_I2C_APPS_CLK			24
#define GCC_BLSP1_QUP3_SPI_APPS_CLK			25
#define GCC_BLSP1_UART1_APPS_CLK			26
#define GCC_BLSP1_UART2_APPS_CLK			27
#define GCC_BTSS_LPO_CLK				28
#define GCC_CMN_BLK_AHB_CLK				29
#define GCC_CMN_BLK_SYS_CLK				30
#define GCC_CRYPTO_AHB_CLK				31
#define GCC_CRYPTO_AXI_CLK				32
#define GCC_CRYPTO_CLK					33
#define GCC_CRYPTO_PPE_CLK				34
#define GCC_DCC_CLK					35
#define GCC_GEPHY_RX_CLK				36
#define GCC_GEPHY_TX_CLK				37
#define GCC_GMAC0_CFG_CLK				38
#define GCC_GMAC0_PTP_CLK				39
#define GCC_GMAC0_RX_CLK				40
#define GCC_GMAC0_SYS_CLK				41
#define GCC_GMAC0_TX_CLK				42
#define GCC_GMAC1_CFG_CLK				43
#define GCC_GMAC1_PTP_CLK				44
#define GCC_GMAC1_RX_CLK				45
#define GCC_GMAC1_SYS_CLK				46
#define GCC_GMAC1_TX_CLK				47
#define GCC_GP1_CLK					48
#define GCC_GP2_CLK					49
#define GCC_GP3_CLK					50
#define GCC_LPASS_CORE_AXIM_CLK				51
#define GCC_LPASS_SWAY_CLK				52
#define GCC_MDIO0_AHB_CLK				53
#define GCC_MDIO1_AHB_CLK				54
#define GCC_PCIE0_AHB_CLK				55
#define GCC_PCIE0_AUX_CLK				56
#define GCC_PCIE0_AXI_M_CLK				57
#define GCC_PCIE0_AXI_S_BRIDGE_CLK			58
#define GCC_PCIE0_AXI_S_CLK				59
#define GCC_PCIE0_PIPE_CLK				60
#define GCC_PCIE1_AHB_CLK				61
#define GCC_PCIE1_AUX_CLK				62
#define GCC_PCIE1_AXI_M_CLK				63
#define GCC_PCIE1_AXI_S_BRIDGE_CLK			64
#define GCC_PCIE1_AXI_S_CLK				65
#define GCC_PCIE1_PIPE_CLK				66
#define GCC_PRNG_AHB_CLK				67
#define GCC_Q6_AXIM_CLK					68
#define GCC_Q6_AXIM2_CLK				69
#define GCC_Q6_AXIS_CLK					70
#define GCC_Q6_AHB_CLK					71
#define GCC_Q6_AHB_S_CLK				72
#define GCC_Q6_TSCTR_1TO2_CLK				73
#define GCC_Q6SS_ATBM_CLK				74
#define GCC_Q6SS_PCLKDBG_CLK				75
#define GCC_Q6SS_TRIG_CLK				76
#define GCC_QDSS_AT_CLK					77
#define GCC_QDSS_CFG_AHB_CLK				78
#define GCC_QDSS_DAP_AHB_CLK				79
#define GCC_QDSS_DAP_CLK				80
#define GCC_QDSS_ETR_USB_CLK				81
#define GCC_QDSS_EUD_AT_CLK				82
#define GCC_QDSS_STM_CLK				83
#define GCC_QDSS_TRACECLKIN_CLK				84
#define GCC_QDSS_TSCTR_DIV8_CLK				85
#define GCC_QPIC_AHB_CLK				86
#define GCC_QPIC_CLK					87
#define GCC_QPIC_IO_MACRO_CLK				88
#define GCC_SDCC1_AHB_CLK				89
#define GCC_SDCC1_APPS_CLK				90
#define GCC_SLEEP_CLK_SRC				91
#define GCC_SNOC_GMAC0_AHB_CLK				92
#define GCC_SNOC_GMAC0_AXI_CLK				93
#define GCC_SNOC_GMAC1_AHB_CLK				94
#define GCC_SNOC_GMAC1_AXI_CLK				95
#define GCC_SNOC_LPASS_AXIM_CLK				96
#define GCC_SNOC_LPASS_SWAY_CLK				97
#define GCC_SNOC_UBI0_AXI_CLK				98
#define GCC_SYS_NOC_PCIE0_AXI_CLK			99
#define GCC_SYS_NOC_PCIE1_AXI_CLK			100
#define GCC_SYS_NOC_QDSS_STM_AXI_CLK			101
#define GCC_SYS_NOC_USB0_AXI_CLK			102
#define GCC_SYS_NOC_WCSS_AHB_CLK			103
#define GCC_UBI0_AXI_CLK				104
#define GCC_UBI0_CFG_CLK				105
#define GCC_UBI0_CORE_CLK				106
#define GCC_UBI0_DBG_CLK				107
#define GCC_UBI0_NC_AXI_CLK				108
#define GCC_UBI0_UTCM_CLK				109
#define GCC_UNIPHY_AHB_CLK				110
#define GCC_UNIPHY_RX_CLK				111
#define GCC_UNIPHY_SYS_CLK				112
#define GCC_UNIPHY_TX_CLK				113
#define GCC_USB0_AUX_CLK				114
#define GCC_USB0_EUD_AT_CLK				115
#define GCC_USB0_LFPS_CLK				116
#define GCC_USB0_MASTER_CLK				117
#define GCC_USB0_MOCK_UTMI_CLK				118
#define GCC_USB0_PHY_CFG_AHB_CLK			119
#define GCC_USB0_SLEEP_CLK				120
#define GCC_WCSS_ACMT_CLK				121
#define GCC_WCSS_AHB_S_CLK				122
#define GCC_WCSS_AXI_M_CLK				123
#define GCC_WCSS_AXI_S_CLK				124
#define GCC_WCSS_DBG_IFC_APB_BDG_CLK			125
#define GCC_WCSS_DBG_IFC_APB_CLK			126
#define GCC_WCSS_DBG_IFC_ATB_BDG_CLK			127
#define GCC_WCSS_DBG_IFC_ATB_CLK			128
#define GCC_WCSS_DBG_IFC_DAPBUS_BDG_CLK			129
#define GCC_WCSS_DBG_IFC_DAPBUS_CLK			130
#define GCC_WCSS_DBG_IFC_NTS_BDG_CLK			131
#define GCC_WCSS_DBG_IFC_NTS_CLK			132
#define GCC_WCSS_ECAHB_CLK				133
#define GCC_XO_CLK					134
#define GCC_XO_CLK_SRC					135
#define GMAC0_RX_CLK_SRC				136
#define GMAC0_TX_CLK_SRC				137
#define GMAC1_RX_CLK_SRC				138
#define GMAC1_TX_CLK_SRC				139
#define GMAC_CLK_SRC					140
#define GP1_CLK_SRC					141
#define GP2_CLK_SRC					142
#define GP3_CLK_SRC					143
#define LPASS_AXIM_CLK_SRC				144
#define LPASS_SWAY_CLK_SRC				145
#define PCIE0_AUX_CLK_SRC				146
#define PCIE0_AXI_CLK_SRC				147
#define PCIE1_AUX_CLK_SRC				148
#define PCIE1_AXI_CLK_SRC				149
#define PCNOC_BFDCD_CLK_SRC				150
#define Q6_AXI_CLK_SRC					151
#define QDSS_AT_CLK_SRC					152
#define QDSS_STM_CLK_SRC				153
#define QDSS_TSCTR_CLK_SRC				154
#define QDSS_TRACECLKIN_CLK_SRC				155
#define QPIC_IO_MACRO_CLK_SRC				156
#define SDCC1_APPS_CLK_SRC				157
#define SYSTEM_NOC_BFDCD_CLK_SRC			158
#define UBI0_AXI_CLK_SRC				159
#define UBI0_CORE_CLK_SRC				160
#define USB0_AUX_CLK_SRC				161
#define USB0_LFPS_CLK_SRC				162
#define USB0_MASTER_CLK_SRC				163
#define USB0_MOCK_UTMI_CLK_SRC				164
#define WCSS_AHB_CLK_SRC				165
#define PCIE0_PIPE_CLK_SRC				166
#define PCIE1_PIPE_CLK_SRC				167
#define USB0_PIPE_CLK_SRC				168
#define GCC_USB0_PIPE_CLK				169
#define GMAC0_RX_DIV_CLK_SRC				170
#define GMAC0_TX_DIV_CLK_SRC				171
#define GMAC1_RX_DIV_CLK_SRC				172
#define GMAC1_TX_DIV_CLK_SRC				173
#endif
