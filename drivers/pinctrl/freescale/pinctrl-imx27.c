// SPDX-License-Identifier: GPL-2.0+
//
// imx27 pinctrl driver based on imx pinmux core
//
// Copyright (C) 2013 Pengutronix
//
// Author: Markus Pargmann <mpa@pengutronix.de>

#include <freax/err.h>
#include <freax/init.h>
#include <freax/io.h>
#include <freax/mod_devicetable.h>
#include <freax/platform_device.h>
#include <freax/pinctrl/pinctrl.h>

#include "pinctrl-imx1.h"

#define PAD_ID(port, pin) (port*32 + pin)
#define PA 0
#define PB 1
#define PC 2
#define PD 3
#define PE 4
#define PF 5

enum imx27_pads {
	MX27_PAD_USBH2_CLK = PAD_ID(PA, 0),
	MX27_PAD_USBH2_DIR = PAD_ID(PA, 1),
	MX27_PAD_USBH2_DATA7 = PAD_ID(PA, 2),
	MX27_PAD_USBH2_NXT = PAD_ID(PA, 3),
	MX27_PAD_USBH2_STP = PAD_ID(PA, 4),
	MX27_PAD_LSCLK = PAD_ID(PA, 5),
	MX27_PAD_LD0 = PAD_ID(PA, 6),
	MX27_PAD_LD1 = PAD_ID(PA, 7),
	MX27_PAD_LD2 = PAD_ID(PA, 8),
	MX27_PAD_LD3 = PAD_ID(PA, 9),
	MX27_PAD_LD4 = PAD_ID(PA, 10),
	MX27_PAD_LD5 = PAD_ID(PA, 11),
	MX27_PAD_LD6 = PAD_ID(PA, 12),
	MX27_PAD_LD7 = PAD_ID(PA, 13),
	MX27_PAD_LD8 = PAD_ID(PA, 14),
	MX27_PAD_LD9 = PAD_ID(PA, 15),
	MX27_PAD_LD10 = PAD_ID(PA, 16),
	MX27_PAD_LD11 = PAD_ID(PA, 17),
	MX27_PAD_LD12 = PAD_ID(PA, 18),
	MX27_PAD_LD13 = PAD_ID(PA, 19),
	MX27_PAD_LD14 = PAD_ID(PA, 20),
	MX27_PAD_LD15 = PAD_ID(PA, 21),
	MX27_PAD_LD16 = PAD_ID(PA, 22),
	MX27_PAD_LD17 = PAD_ID(PA, 23),
	MX27_PAD_REV = PAD_ID(PA, 24),
	MX27_PAD_CLS = PAD_ID(PA, 25),
	MX27_PAD_PS = PAD_ID(PA, 26),
	MX27_PAD_SPL_SPR = PAD_ID(PA, 27),
	MX27_PAD_HSYNC = PAD_ID(PA, 28),
	MX27_PAD_VSYNC = PAD_ID(PA, 29),
	MX27_PAD_CONTRAST = PAD_ID(PA, 30),
	MX27_PAD_OE_ACD = PAD_ID(PA, 31),

	MX27_PAD_SD2_D0 = PAD_ID(PB, 4),
	MX27_PAD_SD2_D1 = PAD_ID(PB, 5),
	MX27_PAD_SD2_D2 = PAD_ID(PB, 6),
	MX27_PAD_SD2_D3 = PAD_ID(PB, 7),
	MX27_PAD_SD2_CMD = PAD_ID(PB, 8),
	MX27_PAD_SD2_CLK = PAD_ID(PB, 9),
	MX27_PAD_CSI_D0 = PAD_ID(PB, 10),
	MX27_PAD_CSI_D1 = PAD_ID(PB, 11),
	MX27_PAD_CSI_D2 = PAD_ID(PB, 12),
	MX27_PAD_CSI_D3 = PAD_ID(PB, 13),
	MX27_PAD_CSI_D4 = PAD_ID(PB, 14),
	MX27_PAD_CSI_MCLK = PAD_ID(PB, 15),
	MX27_PAD_CSI_PIXCLK = PAD_ID(PB, 16),
	MX27_PAD_CSI_D5 = PAD_ID(PB, 17),
	MX27_PAD_CSI_D6 = PAD_ID(PB, 18),
	MX27_PAD_CSI_D7 = PAD_ID(PB, 19),
	MX27_PAD_CSI_VSYNC = PAD_ID(PB, 20),
	MX27_PAD_CSI_HSYNC = PAD_ID(PB, 21),
	MX27_PAD_USBH1_SUSP = PAD_ID(PB, 22),
	MX27_PAD_USB_PWR = PAD_ID(PB, 23),
	MX27_PAD_USB_OC_B = PAD_ID(PB, 24),
	MX27_PAD_USBH1_RCV = PAD_ID(PB, 25),
	MX27_PAD_USBH1_FS = PAD_ID(PB, 26),
	MX27_PAD_USBH1_OE_B = PAD_ID(PB, 27),
	MX27_PAD_USBH1_TXDM = PAD_ID(PB, 28),
	MX27_PAD_USBH1_TXDP = PAD_ID(PB, 29),
	MX27_PAD_USBH1_RXDM = PAD_ID(PB, 30),
	MX27_PAD_USBH1_RXDP = PAD_ID(PB, 31),

	MX27_PAD_I2C2_SDA = PAD_ID(PC, 5),
	MX27_PAD_I2C2_SCL = PAD_ID(PC, 6),
	MX27_PAD_USBOTG_DATA5 = PAD_ID(PC, 7),
	MX27_PAD_USBOTG_DATA6 = PAD_ID(PC, 8),
	MX27_PAD_USBOTG_DATA0 = PAD_ID(PC, 9),
	MX27_PAD_USBOTG_DATA2 = PAD_ID(PC, 10),
	MX27_PAD_USBOTG_DATA1 = PAD_ID(PC, 11),
	MX27_PAD_USBOTG_DATA4 = PAD_ID(PC, 12),
	MX27_PAD_USBOTG_DATA3 = PAD_ID(PC, 13),
	MX27_PAD_TOUT = PAD_ID(PC, 14),
	MX27_PAD_TIN = PAD_ID(PC, 15),
	MX27_PAD_SSI4_FS = PAD_ID(PC, 16),
	MX27_PAD_SSI4_RXDAT = PAD_ID(PC, 17),
	MX27_PAD_SSI4_TXDAT = PAD_ID(PC, 18),
	MX27_PAD_SSI4_CLK = PAD_ID(PC, 19),
	MX27_PAD_SSI1_FS = PAD_ID(PC, 20),
	MX27_PAD_SSI1_RXDAT = PAD_ID(PC, 21),
	MX27_PAD_SSI1_TXDAT = PAD_ID(PC, 22),
	MX27_PAD_SSI1_CLK = PAD_ID(PC, 23),
	MX27_PAD_SSI2_FS = PAD_ID(PC, 24),
	MX27_PAD_SSI2_RXDAT = PAD_ID(PC, 25),
	MX27_PAD_SSI2_TXDAT = PAD_ID(PC, 26),
	MX27_PAD_SSI2_CLK = PAD_ID(PC, 27),
	MX27_PAD_SSI3_FS = PAD_ID(PC, 28),
	MX27_PAD_SSI3_RXDAT = PAD_ID(PC, 29),
	MX27_PAD_SSI3_TXDAT = PAD_ID(PC, 30),
	MX27_PAD_SSI3_CLK = PAD_ID(PC, 31),

	MX27_PAD_SD3_CMD = PAD_ID(PD, 0),
	MX27_PAD_SD3_CLK = PAD_ID(PD, 1),
	MX27_PAD_ATA_DATA0 = PAD_ID(PD, 2),
	MX27_PAD_ATA_DATA1 = PAD_ID(PD, 3),
	MX27_PAD_ATA_DATA2 = PAD_ID(PD, 4),
	MX27_PAD_ATA_DATA3 = PAD_ID(PD, 5),
	MX27_PAD_ATA_DATA4 = PAD_ID(PD, 6),
	MX27_PAD_ATA_DATA5 = PAD_ID(PD, 7),
	MX27_PAD_ATA_DATA6 = PAD_ID(PD, 8),
	MX27_PAD_ATA_DATA7 = PAD_ID(PD, 9),
	MX27_PAD_ATA_DATA8 = PAD_ID(PD, 10),
	MX27_PAD_ATA_DATA9 = PAD_ID(PD, 11),
	MX27_PAD_ATA_DATA10 = PAD_ID(PD, 12),
	MX27_PAD_ATA_DATA11 = PAD_ID(PD, 13),
	MX27_PAD_ATA_DATA12 = PAD_ID(PD, 14),
	MX27_PAD_ATA_DATA13 = PAD_ID(PD, 15),
	MX27_PAD_ATA_DATA14 = PAD_ID(PD, 16),
	MX27_PAD_I2C_DATA = PAD_ID(PD, 17),
	MX27_PAD_I2C_CLK = PAD_ID(PD, 18),
	MX27_PAD_CSPI2_SS2 = PAD_ID(PD, 19),
	MX27_PAD_CSPI2_SS1 = PAD_ID(PD, 20),
	MX27_PAD_CSPI2_SS0 = PAD_ID(PD, 21),
	MX27_PAD_CSPI2_SCLK = PAD_ID(PD, 22),
	MX27_PAD_CSPI2_MISO = PAD_ID(PD, 23),
	MX27_PAD_CSPI2_MOSI = PAD_ID(PD, 24),
	MX27_PAD_CSPI1_RDY = PAD_ID(PD, 25),
	MX27_PAD_CSPI1_SS2 = PAD_ID(PD, 26),
	MX27_PAD_CSPI1_SS1 = PAD_ID(PD, 27),
	MX27_PAD_CSPI1_SS0 = PAD_ID(PD, 28),
	MX27_PAD_CSPI1_SCLK = PAD_ID(PD, 29),
	MX27_PAD_CSPI1_MISO = PAD_ID(PD, 30),
	MX27_PAD_CSPI1_MOSI = PAD_ID(PD, 31),

	MX27_PAD_USBOTG_NXT = PAD_ID(PE, 0),
	MX27_PAD_USBOTG_STP = PAD_ID(PE, 1),
	MX27_PAD_USBOTG_DIR = PAD_ID(PE, 2),
	MX27_PAD_UART2_CTS = PAD_ID(PE, 3),
	MX27_PAD_UART2_RTS = PAD_ID(PE, 4),
	MX27_PAD_PWMO = PAD_ID(PE, 5),
	MX27_PAD_UART2_TXD = PAD_ID(PE, 6),
	MX27_PAD_UART2_RXD = PAD_ID(PE, 7),
	MX27_PAD_UART3_TXD = PAD_ID(PE, 8),
	MX27_PAD_UART3_RXD = PAD_ID(PE, 9),
	MX27_PAD_UART3_CTS = PAD_ID(PE, 10),
	MX27_PAD_UART3_RTS = PAD_ID(PE, 11),
	MX27_PAD_UART1_TXD = PAD_ID(PE, 12),
	MX27_PAD_UART1_RXD = PAD_ID(PE, 13),
	MX27_PAD_UART1_CTS = PAD_ID(PE, 14),
	MX27_PAD_UART1_RTS = PAD_ID(PE, 15),
	MX27_PAD_RTCK = PAD_ID(PE, 16),
	MX27_PAD_RESET_OUT_B = PAD_ID(PE, 17),
	MX27_PAD_SD1_D0 = PAD_ID(PE, 18),
	MX27_PAD_SD1_D1 = PAD_ID(PE, 19),
	MX27_PAD_SD1_D2 = PAD_ID(PE, 20),
	MX27_PAD_SD1_D3 = PAD_ID(PE, 21),
	MX27_PAD_SD1_CMD = PAD_ID(PE, 22),
	MX27_PAD_SD1_CLK = PAD_ID(PE, 23),
	MX27_PAD_USBOTG_CLK = PAD_ID(PE, 24),
	MX27_PAD_USBOTG_DATA7 = PAD_ID(PE, 25),

	MX27_PAD_NFRB = PAD_ID(PF, 0),
	MX27_PAD_NFCLE = PAD_ID(PF, 1),
	MX27_PAD_NFWP_B = PAD_ID(PF, 2),
	MX27_PAD_NFCE_B = PAD_ID(PF, 3),
	MX27_PAD_NFALE = PAD_ID(PF, 4),
	MX27_PAD_NFRE_B = PAD_ID(PF, 5),
	MX27_PAD_NFWE_B = PAD_ID(PF, 6),
	MX27_PAD_PC_POE = PAD_ID(PF, 7),
	MX27_PAD_PC_RW_B = PAD_ID(PF, 8),
	MX27_PAD_IOIS16 = PAD_ID(PF, 9),
	MX27_PAD_PC_RST = PAD_ID(PF, 10),
	MX27_PAD_PC_BVD2 = PAD_ID(PF, 11),
	MX27_PAD_PC_BVD1 = PAD_ID(PF, 12),
	MX27_PAD_PC_VS2 = PAD_ID(PF, 13),
	MX27_PAD_PC_VS1 = PAD_ID(PF, 14),
	MX27_PAD_CLKO = PAD_ID(PF, 15),
	MX27_PAD_PC_PWRON = PAD_ID(PF, 16),
	MX27_PAD_PC_READY = PAD_ID(PF, 17),
	MX27_PAD_PC_WAIT_B = PAD_ID(PF, 18),
	MX27_PAD_PC_CD2_B = PAD_ID(PF, 19),
	MX27_PAD_PC_CD1_B = PAD_ID(PF, 20),
	MX27_PAD_CS4_B = PAD_ID(PF, 21),
	MX27_PAD_CS5_B = PAD_ID(PF, 22),
	MX27_PAD_ATA_DATA15 = PAD_ID(PF, 23),
};

/* Pad names for the pinmux subsystem */
static const struct pinctrl_pin_desc imx27_pinctrl_pads[] = {
	IMX_PINCTRL_PIN(MX27_PAD_USBH2_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_USBH2_DIR),
	IMX_PINCTRL_PIN(MX27_PAD_USBH2_DATA7),
	IMX_PINCTRL_PIN(MX27_PAD_USBH2_NXT),
	IMX_PINCTRL_PIN(MX27_PAD_USBH2_STP),
	IMX_PINCTRL_PIN(MX27_PAD_LSCLK),
	IMX_PINCTRL_PIN(MX27_PAD_LD0),
	IMX_PINCTRL_PIN(MX27_PAD_LD1),
	IMX_PINCTRL_PIN(MX27_PAD_LD2),
	IMX_PINCTRL_PIN(MX27_PAD_LD3),
	IMX_PINCTRL_PIN(MX27_PAD_LD4),
	IMX_PINCTRL_PIN(MX27_PAD_LD5),
	IMX_PINCTRL_PIN(MX27_PAD_LD6),
	IMX_PINCTRL_PIN(MX27_PAD_LD7),
	IMX_PINCTRL_PIN(MX27_PAD_LD8),
	IMX_PINCTRL_PIN(MX27_PAD_LD9),
	IMX_PINCTRL_PIN(MX27_PAD_LD10),
	IMX_PINCTRL_PIN(MX27_PAD_LD11),
	IMX_PINCTRL_PIN(MX27_PAD_LD12),
	IMX_PINCTRL_PIN(MX27_PAD_LD13),
	IMX_PINCTRL_PIN(MX27_PAD_LD14),
	IMX_PINCTRL_PIN(MX27_PAD_LD15),
	IMX_PINCTRL_PIN(MX27_PAD_LD16),
	IMX_PINCTRL_PIN(MX27_PAD_LD17),
	IMX_PINCTRL_PIN(MX27_PAD_REV),
	IMX_PINCTRL_PIN(MX27_PAD_CLS),
	IMX_PINCTRL_PIN(MX27_PAD_PS),
	IMX_PINCTRL_PIN(MX27_PAD_SPL_SPR),
	IMX_PINCTRL_PIN(MX27_PAD_HSYNC),
	IMX_PINCTRL_PIN(MX27_PAD_VSYNC),
	IMX_PINCTRL_PIN(MX27_PAD_CONTRAST),
	IMX_PINCTRL_PIN(MX27_PAD_OE_ACD),

	IMX_PINCTRL_PIN(MX27_PAD_SD2_D0),
	IMX_PINCTRL_PIN(MX27_PAD_SD2_D1),
	IMX_PINCTRL_PIN(MX27_PAD_SD2_D2),
	IMX_PINCTRL_PIN(MX27_PAD_SD2_D3),
	IMX_PINCTRL_PIN(MX27_PAD_SD2_CMD),
	IMX_PINCTRL_PIN(MX27_PAD_SD2_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_D0),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_D1),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_D2),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_D3),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_D4),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_MCLK),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_PIXCLK),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_D5),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_D6),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_D7),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_VSYNC),
	IMX_PINCTRL_PIN(MX27_PAD_CSI_HSYNC),
	IMX_PINCTRL_PIN(MX27_PAD_USBH1_SUSP),
	IMX_PINCTRL_PIN(MX27_PAD_USB_PWR),
	IMX_PINCTRL_PIN(MX27_PAD_USB_OC_B),
	IMX_PINCTRL_PIN(MX27_PAD_USBH1_RCV),
	IMX_PINCTRL_PIN(MX27_PAD_USBH1_FS),
	IMX_PINCTRL_PIN(MX27_PAD_USBH1_OE_B),
	IMX_PINCTRL_PIN(MX27_PAD_USBH1_TXDM),
	IMX_PINCTRL_PIN(MX27_PAD_USBH1_TXDP),
	IMX_PINCTRL_PIN(MX27_PAD_USBH1_RXDM),
	IMX_PINCTRL_PIN(MX27_PAD_USBH1_RXDP),

	IMX_PINCTRL_PIN(MX27_PAD_I2C2_SDA),
	IMX_PINCTRL_PIN(MX27_PAD_I2C2_SCL),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DATA5),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DATA6),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DATA0),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DATA2),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DATA1),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DATA4),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DATA3),
	IMX_PINCTRL_PIN(MX27_PAD_TOUT),
	IMX_PINCTRL_PIN(MX27_PAD_TIN),
	IMX_PINCTRL_PIN(MX27_PAD_SSI4_FS),
	IMX_PINCTRL_PIN(MX27_PAD_SSI4_RXDAT),
	IMX_PINCTRL_PIN(MX27_PAD_SSI4_TXDAT),
	IMX_PINCTRL_PIN(MX27_PAD_SSI4_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_SSI1_FS),
	IMX_PINCTRL_PIN(MX27_PAD_SSI1_RXDAT),
	IMX_PINCTRL_PIN(MX27_PAD_SSI1_TXDAT),
	IMX_PINCTRL_PIN(MX27_PAD_SSI1_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_SSI2_FS),
	IMX_PINCTRL_PIN(MX27_PAD_SSI2_RXDAT),
	IMX_PINCTRL_PIN(MX27_PAD_SSI2_TXDAT),
	IMX_PINCTRL_PIN(MX27_PAD_SSI2_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_SSI3_FS),
	IMX_PINCTRL_PIN(MX27_PAD_SSI3_RXDAT),
	IMX_PINCTRL_PIN(MX27_PAD_SSI3_TXDAT),
	IMX_PINCTRL_PIN(MX27_PAD_SSI3_CLK),

	IMX_PINCTRL_PIN(MX27_PAD_SD3_CMD),
	IMX_PINCTRL_PIN(MX27_PAD_SD3_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA0),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA1),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA2),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA3),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA4),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA5),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA6),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA7),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA8),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA9),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA10),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA11),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA12),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA13),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA14),
	IMX_PINCTRL_PIN(MX27_PAD_I2C_DATA),
	IMX_PINCTRL_PIN(MX27_PAD_I2C_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI2_SS2),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI2_SS1),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI2_SS0),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI2_SCLK),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI2_MISO),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI2_MOSI),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI1_RDY),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI1_SS2),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI1_SS1),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI1_SS0),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI1_SCLK),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI1_MISO),
	IMX_PINCTRL_PIN(MX27_PAD_CSPI1_MOSI),

	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_NXT),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_STP),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DIR),
	IMX_PINCTRL_PIN(MX27_PAD_UART2_CTS),
	IMX_PINCTRL_PIN(MX27_PAD_UART2_RTS),
	IMX_PINCTRL_PIN(MX27_PAD_PWMO),
	IMX_PINCTRL_PIN(MX27_PAD_UART2_TXD),
	IMX_PINCTRL_PIN(MX27_PAD_UART2_RXD),
	IMX_PINCTRL_PIN(MX27_PAD_UART3_TXD),
	IMX_PINCTRL_PIN(MX27_PAD_UART3_RXD),
	IMX_PINCTRL_PIN(MX27_PAD_UART3_CTS),
	IMX_PINCTRL_PIN(MX27_PAD_UART3_RTS),
	IMX_PINCTRL_PIN(MX27_PAD_UART1_TXD),
	IMX_PINCTRL_PIN(MX27_PAD_UART1_RXD),
	IMX_PINCTRL_PIN(MX27_PAD_UART1_CTS),
	IMX_PINCTRL_PIN(MX27_PAD_UART1_RTS),
	IMX_PINCTRL_PIN(MX27_PAD_RTCK),
	IMX_PINCTRL_PIN(MX27_PAD_RESET_OUT_B),
	IMX_PINCTRL_PIN(MX27_PAD_SD1_D0),
	IMX_PINCTRL_PIN(MX27_PAD_SD1_D1),
	IMX_PINCTRL_PIN(MX27_PAD_SD1_D2),
	IMX_PINCTRL_PIN(MX27_PAD_SD1_D3),
	IMX_PINCTRL_PIN(MX27_PAD_SD1_CMD),
	IMX_PINCTRL_PIN(MX27_PAD_SD1_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_CLK),
	IMX_PINCTRL_PIN(MX27_PAD_USBOTG_DATA7),

	IMX_PINCTRL_PIN(MX27_PAD_NFRB),
	IMX_PINCTRL_PIN(MX27_PAD_NFCLE),
	IMX_PINCTRL_PIN(MX27_PAD_NFWP_B),
	IMX_PINCTRL_PIN(MX27_PAD_NFCE_B),
	IMX_PINCTRL_PIN(MX27_PAD_NFALE),
	IMX_PINCTRL_PIN(MX27_PAD_NFRE_B),
	IMX_PINCTRL_PIN(MX27_PAD_NFWE_B),
	IMX_PINCTRL_PIN(MX27_PAD_PC_POE),
	IMX_PINCTRL_PIN(MX27_PAD_PC_RW_B),
	IMX_PINCTRL_PIN(MX27_PAD_IOIS16),
	IMX_PINCTRL_PIN(MX27_PAD_PC_RST),
	IMX_PINCTRL_PIN(MX27_PAD_PC_BVD2),
	IMX_PINCTRL_PIN(MX27_PAD_PC_BVD1),
	IMX_PINCTRL_PIN(MX27_PAD_PC_VS2),
	IMX_PINCTRL_PIN(MX27_PAD_PC_VS1),
	IMX_PINCTRL_PIN(MX27_PAD_CLKO),
	IMX_PINCTRL_PIN(MX27_PAD_PC_PWRON),
	IMX_PINCTRL_PIN(MX27_PAD_PC_READY),
	IMX_PINCTRL_PIN(MX27_PAD_PC_WAIT_B),
	IMX_PINCTRL_PIN(MX27_PAD_PC_CD2_B),
	IMX_PINCTRL_PIN(MX27_PAD_PC_CD1_B),
	IMX_PINCTRL_PIN(MX27_PAD_CS4_B),
	IMX_PINCTRL_PIN(MX27_PAD_CS5_B),
	IMX_PINCTRL_PIN(MX27_PAD_ATA_DATA15),
};

static struct imx1_pinctrl_soc_info imx27_pinctrl_info = {
	.pins = imx27_pinctrl_pads,
	.npins = ARRAY_SIZE(imx27_pinctrl_pads),
};

static const struct of_device_id imx27_pinctrl_of_match[] = {
	{ .compatible = "fsl,imx27-iomuxc", },
	{ /* sentinel */ }
};

static int imx27_pinctrl_probe(struct platform_device *pdev)
{
	return imx1_pinctrl_core_probe(pdev, &imx27_pinctrl_info);
}

static struct platform_driver imx27_pinctrl_driver = {
	.driver = {
		.name = "imx27-pinctrl",
		.of_match_table = imx27_pinctrl_of_match,
		.suppress_bind_attrs = true,
	},
	.probe = imx27_pinctrl_probe,
};

static int __init imx27_pinctrl_init(void)
{
	return platform_driver_register(&imx27_pinctrl_driver);
}
arch_initcall(imx27_pinctrl_init);
