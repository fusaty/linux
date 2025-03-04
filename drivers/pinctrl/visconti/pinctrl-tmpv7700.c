// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 TOSHIBA CORPORATION
 * Copyright (c) 2020 Toshiba Electronic Devices & Storage Corporation
 * Copyright (c) 2020 Nobuhiro Iwamatsu <nobuhiro1.iwamatsu@toshiba.co.jp>
 */

#include <freax/init.h>
#include <freax/io.h>
#include <freax/of.h>
#include <freax/platform_device.h>
#include <freax/pinctrl/pinctrl.h>
#include "pinctrl-common.h"

#define tmpv7700_MAGIC_NUM 0x4932f70e

/* register offset */
#define REG_KEY_CTRL	0x0000
#define REG_KEY_CMD	0x0004
#define REG_PINMUX1	0x3000
#define REG_PINMUX2	0x3004
#define REG_PINMUX3	0x3008
#define REG_PINMUX4	0x300c
#define REG_PINMUX5	0x3010
#define REG_IOSET	0x3014
#define REG_IO_VSEL	0x3018
#define REG_IO_DSEL1	0x301c
#define REG_IO_DSEL2	0x3020
#define REG_IO_DSEL3	0x3024
#define REG_IO_DSEL4	0x3028
#define REG_IO_DSEL5	0x302c
#define REG_IO_DSEL6	0x3030
#define REG_IO_DSEL7	0x3034
#define REG_IO_DSEL8	0x3038
#define REG_IO_PUDE1	0x303c
#define REG_IO_PUDE2	0x3040
#define REG_IO_PUDSEL1	0x3044
#define REG_IO_PUDSEL2	0x3048

/* PIN */
static const struct visconti_desc_pin pins_tmpv7700[] = {
	VISCONTI_PIN(PINCTRL_PIN(0, "gpio0"), REG_IO_DSEL4, 24,
		    REG_IO_PUDE1, REG_IO_PUDSEL1, 30),
	VISCONTI_PIN(PINCTRL_PIN(1, "gpio1"), REG_IO_DSEL4, 28,
		    REG_IO_PUDE1, REG_IO_PUDSEL1, 31),
	VISCONTI_PIN(PINCTRL_PIN(2, "gpio2"), REG_IO_DSEL5, 0,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 0),
	VISCONTI_PIN(PINCTRL_PIN(3, "gpio3"), REG_IO_DSEL5, 4,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 1),
	VISCONTI_PIN(PINCTRL_PIN(4, "gpio4"), REG_IO_DSEL5, 8,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 2),
	VISCONTI_PIN(PINCTRL_PIN(5, "gpio5"), REG_IO_DSEL5, 12,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 3),
	VISCONTI_PIN(PINCTRL_PIN(6, "gpio6"), REG_IO_DSEL5, 16,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 4),
	VISCONTI_PIN(PINCTRL_PIN(7, "gpio7"), REG_IO_DSEL5, 20,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 5),
	VISCONTI_PIN(PINCTRL_PIN(8, "gpio8"), REG_IO_DSEL5, 24,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 6),
	VISCONTI_PIN(PINCTRL_PIN(9, "gpio9"), REG_IO_DSEL5, 28,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 7),
	VISCONTI_PIN(PINCTRL_PIN(10, "gpio10"), REG_IO_DSEL6, 0,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 8),
	VISCONTI_PIN(PINCTRL_PIN(11, "gpio11"), REG_IO_DSEL6, 4,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 9),
	VISCONTI_PIN(PINCTRL_PIN(12, "gpio12"), REG_IO_DSEL6, 8,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 10),
	VISCONTI_PIN(PINCTRL_PIN(13, "gpio13"), REG_IO_DSEL6, 12,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 11),
	VISCONTI_PIN(PINCTRL_PIN(14, "gpio14"), REG_IO_DSEL6, 16,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 12),
	VISCONTI_PIN(PINCTRL_PIN(15, "gpio15"), REG_IO_DSEL6, 20,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 13),
	VISCONTI_PIN(PINCTRL_PIN(16, "gpio16"), REG_IO_DSEL6, 24,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 14),
	VISCONTI_PIN(PINCTRL_PIN(17, "gpio17"), REG_IO_DSEL6, 28,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 15),
	VISCONTI_PIN(PINCTRL_PIN(18, "gpio18"), REG_IO_DSEL7, 0,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 16),
	VISCONTI_PIN(PINCTRL_PIN(19, "gpio19"), REG_IO_DSEL7, 4,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 17),
	VISCONTI_PIN(PINCTRL_PIN(20, "gpio20"), REG_IO_DSEL7, 8,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 18),
	VISCONTI_PIN(PINCTRL_PIN(21, "gpio21"), REG_IO_DSEL7, 12,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 19),
	VISCONTI_PIN(PINCTRL_PIN(22, "gpio22"), REG_IO_DSEL7, 16,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 20),
	VISCONTI_PIN(PINCTRL_PIN(23, "gpio23"), REG_IO_DSEL7, 20,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 21),
	VISCONTI_PIN(PINCTRL_PIN(24, "gpio24"), REG_IO_DSEL7, 24,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 22),
	VISCONTI_PIN(PINCTRL_PIN(25, "gpio25"), REG_IO_DSEL7, 28,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 23),
	VISCONTI_PIN(PINCTRL_PIN(26, "gpio26"), REG_IO_DSEL8, 0,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 24),
	VISCONTI_PIN(PINCTRL_PIN(27, "gpio27"), REG_IO_DSEL8, 4,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 25),
	VISCONTI_PIN(PINCTRL_PIN(28, "gpio28"), REG_IO_DSEL8, 8,
		    REG_IO_PUDE2, REG_IO_PUDSEL2, 26),
	VISCONTI_PIN(PINCTRL_PIN(29, "gpio29"), REG_IO_DSEL4, 8,
		    REG_IO_PUDE1, REG_IO_PUDSEL1, 26),
	VISCONTI_PIN(PINCTRL_PIN(30, "gpio30"), REG_IO_DSEL4, 4,
		    REG_IO_PUDE1, REG_IO_PUDSEL1, 25),
	VISCONTI_PIN(PINCTRL_PIN(31, "gpio31"), REG_IO_DSEL4, 0,
		    REG_IO_PUDE1, REG_IO_PUDSEL1, 24),
	VISCONTI_PIN(PINCTRL_PIN(32, "spi_sck"), REG_IO_DSEL4, 12,
		    REG_IO_PUDE1, REG_IO_PUDSEL1, 27),
	VISCONTI_PIN(PINCTRL_PIN(33, "spi_sdo"), REG_IO_DSEL4, 16,
		    REG_IO_PUDE1, REG_IO_PUDSEL1, 28),
	VISCONTI_PIN(PINCTRL_PIN(34, "spi_sdi"), REG_IO_DSEL4, 20,
		    REG_IO_PUDE1, REG_IO_PUDSEL1, 29),
};

/* Group */
VISCONTI_PINS(i2c0, 0, 1);
VISCONTI_PINS(i2c1, 2, 3);
VISCONTI_PINS(i2c2, 12, 13);
VISCONTI_PINS(i2c3, 14, 15);
VISCONTI_PINS(i2c4, 16, 17);
VISCONTI_PINS(i2c5, 18, 19);
VISCONTI_PINS(i2c6, 33, 34);
VISCONTI_PINS(i2c7, 29, 32);
VISCONTI_PINS(i2c8, 30, 31);
VISCONTI_PINS(spi0_cs0, 29);
VISCONTI_PINS(spi0_cs1, 30);
VISCONTI_PINS(spi0_cs2, 31);
VISCONTI_PINS(spi1_cs, 3);
VISCONTI_PINS(spi2_cs, 7);
VISCONTI_PINS(spi3_cs, 11);
VISCONTI_PINS(spi4_cs, 15);
VISCONTI_PINS(spi5_cs, 19);
VISCONTI_PINS(spi6_cs, 27);
VISCONTI_PINS(spi0, 32, 33, 34);
VISCONTI_PINS(spi1, 0, 1, 2);
VISCONTI_PINS(spi2, 4, 5, 6);
VISCONTI_PINS(spi3, 8, 9, 10);
VISCONTI_PINS(spi4, 12, 13, 14);
VISCONTI_PINS(spi5, 16, 17, 18);
VISCONTI_PINS(spi6, 24, 25, 26);
VISCONTI_PINS(uart0, 4, 5, 6, 7);
VISCONTI_PINS(uart1, 8, 9, 10, 11);
VISCONTI_PINS(uart2, 12, 13, 14, 15);
VISCONTI_PINS(uart3, 16, 17, 18, 19);
VISCONTI_PINS(pwm0_gpio4, 4);
VISCONTI_PINS(pwm1_gpio5, 5);
VISCONTI_PINS(pwm2_gpio6, 6);
VISCONTI_PINS(pwm3_gpio7, 7);
VISCONTI_PINS(pwm0_gpio8, 8);
VISCONTI_PINS(pwm1_gpio9, 9);
VISCONTI_PINS(pwm2_gpio10, 10);
VISCONTI_PINS(pwm3_gpio11, 11);
VISCONTI_PINS(pwm0_gpio12, 12);
VISCONTI_PINS(pwm1_gpio13, 13);
VISCONTI_PINS(pwm2_gpio14, 14);
VISCONTI_PINS(pwm3_gpio15, 15);
VISCONTI_PINS(pwm0_gpio16, 16);
VISCONTI_PINS(pwm1_gpio17, 17);
VISCONTI_PINS(pwm2_gpio18, 18);
VISCONTI_PINS(pwm3_gpio19, 19);
VISCONTI_PINS(pcmif_out, 20, 21, 22);
VISCONTI_PINS(pcmif_in, 24, 25, 26);

static const struct visconti_pin_group groups_tmpv7700[] = {
	VISCONTI_PIN_GROUP(i2c0, REG_PINMUX2, GENMASK(7, 0), 0x00000022),
	VISCONTI_PIN_GROUP(i2c1, REG_PINMUX2, GENMASK(15, 8), 0x00002200),
	VISCONTI_PIN_GROUP(i2c2, REG_PINMUX3, GENMASK(23, 16), 0x00770000),
	VISCONTI_PIN_GROUP(i2c3, REG_PINMUX3, GENMASK(31, 24), 0x77000000),
	VISCONTI_PIN_GROUP(i2c4, REG_PINMUX4, GENMASK(7, 0), 0x00000077),
	VISCONTI_PIN_GROUP(i2c5, REG_PINMUX4, GENMASK(15, 8), 0x00007700),
	VISCONTI_PIN_GROUP(i2c6, REG_PINMUX1, GENMASK(3, 0), 0x0000002),
	VISCONTI_PIN_GROUP(i2c7, REG_PINMUX5, GENMASK(23, 20), 0x00200000),
	VISCONTI_PIN_GROUP(i2c8, REG_PINMUX5, GENMASK(31, 24), 0x22000000),
	VISCONTI_PIN_GROUP(spi0_cs0, REG_PINMUX5, GENMASK(23, 20), 0x00100000),
	VISCONTI_PIN_GROUP(spi0_cs1, REG_PINMUX5, GENMASK(27, 24), 0x01000000),
	VISCONTI_PIN_GROUP(spi0_cs2, REG_PINMUX5, GENMASK(31, 28), 0x10000000),
	VISCONTI_PIN_GROUP(spi1_cs, REG_PINMUX2, GENMASK(15, 12), 0x00001000),
	VISCONTI_PIN_GROUP(spi2_cs, REG_PINMUX2, GENMASK(31, 28), 0x10000000),
	VISCONTI_PIN_GROUP(spi3_cs, REG_PINMUX3, GENMASK(15, 12), 0x00001000),
	VISCONTI_PIN_GROUP(spi4_cs, REG_PINMUX4, GENMASK(31, 28), 0x10000000),
	VISCONTI_PIN_GROUP(spi5_cs, REG_PINMUX4, GENMASK(15, 12), 0x00001000),
	VISCONTI_PIN_GROUP(spi6_cs, REG_PINMUX5, GENMASK(15, 12), 0x00001000),
	VISCONTI_PIN_GROUP(spi0, REG_PINMUX1, GENMASK(3, 0), 0x00000001),
	VISCONTI_PIN_GROUP(spi1, REG_PINMUX2, GENMASK(11, 0), 0x00000111),
	VISCONTI_PIN_GROUP(spi2, REG_PINMUX2, GENMASK(27, 16), 0x01110000),
	VISCONTI_PIN_GROUP(spi3, REG_PINMUX3, GENMASK(11, 0), 0x00000111),
	VISCONTI_PIN_GROUP(spi4, REG_PINMUX3, GENMASK(27, 16), 0x01110000),
	VISCONTI_PIN_GROUP(spi5, REG_PINMUX4, GENMASK(11, 0), 0x00000111),
	VISCONTI_PIN_GROUP(spi6, REG_PINMUX5, GENMASK(11, 0), 0x00000111),
	VISCONTI_PIN_GROUP(uart0, REG_PINMUX2, GENMASK(31, 16), 0x22220000),
	VISCONTI_PIN_GROUP(uart1, REG_PINMUX3, GENMASK(15, 0), 0x00002222),
	VISCONTI_PIN_GROUP(uart2, REG_PINMUX3, GENMASK(31, 16), 0x22220000),
	VISCONTI_PIN_GROUP(uart3, REG_PINMUX4, GENMASK(15, 0), 0x00002222),
	VISCONTI_PIN_GROUP(pwm0_gpio4, REG_PINMUX2, GENMASK(19, 16), 0x00050000),
	VISCONTI_PIN_GROUP(pwm1_gpio5, REG_PINMUX2, GENMASK(23, 20), 0x00500000),
	VISCONTI_PIN_GROUP(pwm2_gpio6, REG_PINMUX2, GENMASK(27, 24), 0x05000000),
	VISCONTI_PIN_GROUP(pwm3_gpio7, REG_PINMUX2, GENMASK(31, 28), 0x50000000),
	VISCONTI_PIN_GROUP(pwm0_gpio8, REG_PINMUX3, GENMASK(3, 0), 0x00000005),
	VISCONTI_PIN_GROUP(pwm1_gpio9, REG_PINMUX3, GENMASK(7, 4), 0x00000050),
	VISCONTI_PIN_GROUP(pwm2_gpio10, REG_PINMUX3, GENMASK(11, 8), 0x00000500),
	VISCONTI_PIN_GROUP(pwm3_gpio11, REG_PINMUX3, GENMASK(15, 12), 0x00005000),
	VISCONTI_PIN_GROUP(pwm0_gpio12, REG_PINMUX3, GENMASK(19, 16), 0x00050000),
	VISCONTI_PIN_GROUP(pwm1_gpio13, REG_PINMUX3, GENMASK(23, 20), 0x00500000),
	VISCONTI_PIN_GROUP(pwm2_gpio14, REG_PINMUX3, GENMASK(27, 24), 0x05000000),
	VISCONTI_PIN_GROUP(pwm3_gpio15, REG_PINMUX3, GENMASK(31, 28), 0x50000000),
	VISCONTI_PIN_GROUP(pwm0_gpio16, REG_PINMUX4, GENMASK(3, 0), 0x00000005),
	VISCONTI_PIN_GROUP(pwm1_gpio17, REG_PINMUX4, GENMASK(7, 4), 0x00000050),
	VISCONTI_PIN_GROUP(pwm2_gpio18, REG_PINMUX4, GENMASK(11, 8), 0x00000500),
	VISCONTI_PIN_GROUP(pwm3_gpio19, REG_PINMUX4, GENMASK(15, 12), 0x00005000),
	VISCONTI_PIN_GROUP(pcmif_out, REG_PINMUX4, GENMASK(27, 16), 0x01110000),
	VISCONTI_PIN_GROUP(pcmif_in, REG_PINMUX5, GENMASK(11, 0), 0x00000222),
};

/* MUX */
VISCONTI_GROUPS(i2c0, "i2c0_grp");
VISCONTI_GROUPS(i2c1, "i2c1_grp");
VISCONTI_GROUPS(i2c2, "i2c2_grp");
VISCONTI_GROUPS(i2c3, "i2c3_grp");
VISCONTI_GROUPS(i2c4, "i2c4_grp");
VISCONTI_GROUPS(i2c5, "i2c5_grp");
VISCONTI_GROUPS(i2c6, "i2c6_grp");
VISCONTI_GROUPS(i2c7, "i2c7_grp");
VISCONTI_GROUPS(i2c8, "i2c8_grp");
VISCONTI_GROUPS(spi0, "spi0_grp", "spi0_cs0_grp",
		"spi0_cs1_grp", "spi0_cs2_grp");
VISCONTI_GROUPS(spi1, "spi1_grp", "spi1_cs_grp");
VISCONTI_GROUPS(spi2, "spi2_grp", "spi2_cs_grp");
VISCONTI_GROUPS(spi3, "spi3_grp", "spi3_cs_grp");
VISCONTI_GROUPS(spi4, "spi4_grp", "spi4_cs_grp");
VISCONTI_GROUPS(spi5, "spi5_grp", "spi5_cs_grp");
VISCONTI_GROUPS(spi6, "spi6_grp", "spi6_cs_grp");
VISCONTI_GROUPS(uart0, "uart0_grp");
VISCONTI_GROUPS(uart1, "uart1_grp");
VISCONTI_GROUPS(uart2, "uart2_grp");
VISCONTI_GROUPS(uart3, "uart3_grp");
VISCONTI_GROUPS(pwm, "pwm0_gpio4_grp", "pwm0_gpio8_grp",
		"pwm0_gpio12_grp", "pwm0_gpio16_grp",
		"pwm1_gpio5_grp", "pwm1_gpio9_grp",
		"pwm1_gpio13_grp", "pwm1_gpio17_grp",
		"pwm2_gpio6_grp", "pwm2_gpio10_grp",
		"pwm2_gpio14_grp", "pwm2_gpio18_grp",
		"pwm3_gpio7_grp", "pwm3_gpio11_grp",
		"pwm3_gpio15_grp", "pwm3_gpio19_grp");
VISCONTI_GROUPS(pcmif_out, "pcmif_out_grp");
VISCONTI_GROUPS(pcmif_in, "pcmif_in_grp");

static const struct visconti_pin_function functions_tmpv7700[] = {
	VISCONTI_PIN_FUNCTION(i2c0),
	VISCONTI_PIN_FUNCTION(i2c1),
	VISCONTI_PIN_FUNCTION(i2c2),
	VISCONTI_PIN_FUNCTION(i2c3),
	VISCONTI_PIN_FUNCTION(i2c4),
	VISCONTI_PIN_FUNCTION(i2c5),
	VISCONTI_PIN_FUNCTION(i2c6),
	VISCONTI_PIN_FUNCTION(i2c7),
	VISCONTI_PIN_FUNCTION(i2c8),
	VISCONTI_PIN_FUNCTION(spi0),
	VISCONTI_PIN_FUNCTION(spi1),
	VISCONTI_PIN_FUNCTION(spi2),
	VISCONTI_PIN_FUNCTION(spi3),
	VISCONTI_PIN_FUNCTION(spi4),
	VISCONTI_PIN_FUNCTION(spi5),
	VISCONTI_PIN_FUNCTION(spi6),
	VISCONTI_PIN_FUNCTION(uart0),
	VISCONTI_PIN_FUNCTION(uart1),
	VISCONTI_PIN_FUNCTION(uart2),
	VISCONTI_PIN_FUNCTION(uart3),
	VISCONTI_PIN_FUNCTION(pwm),
	VISCONTI_PIN_FUNCTION(pcmif_in),
	VISCONTI_PIN_FUNCTION(pcmif_out),
};

/* GPIO MUX */
#define tmpv7700_GPIO_MUX(off, msk)	\
{					\
	.offset = off,			\
	.mask = msk,			\
	.val = 0,			\
}

static const struct visconti_mux gpio_mux_tmpv7700[] = {
	tmpv7700_GPIO_MUX(REG_PINMUX2, GENMASK(3, 0)),
	tmpv7700_GPIO_MUX(REG_PINMUX2, GENMASK(7, 4)),
	tmpv7700_GPIO_MUX(REG_PINMUX2, GENMASK(11, 8)),
	tmpv7700_GPIO_MUX(REG_PINMUX2, GENMASK(15, 12)),
	tmpv7700_GPIO_MUX(REG_PINMUX2, GENMASK(19, 16)),
	tmpv7700_GPIO_MUX(REG_PINMUX2, GENMASK(23, 20)),
	tmpv7700_GPIO_MUX(REG_PINMUX2, GENMASK(27, 24)),
	tmpv7700_GPIO_MUX(REG_PINMUX2, GENMASK(31, 28)),
	tmpv7700_GPIO_MUX(REG_PINMUX3, GENMASK(3, 0)),
	tmpv7700_GPIO_MUX(REG_PINMUX3, GENMASK(7, 4)),
	tmpv7700_GPIO_MUX(REG_PINMUX3, GENMASK(11, 8)),
	tmpv7700_GPIO_MUX(REG_PINMUX3, GENMASK(15, 12)),
	tmpv7700_GPIO_MUX(REG_PINMUX3, GENMASK(19, 16)),
	tmpv7700_GPIO_MUX(REG_PINMUX3, GENMASK(23, 20)),
	tmpv7700_GPIO_MUX(REG_PINMUX3, GENMASK(27, 24)),
	tmpv7700_GPIO_MUX(REG_PINMUX3, GENMASK(31, 28)),
	tmpv7700_GPIO_MUX(REG_PINMUX4, GENMASK(3, 0)),
	tmpv7700_GPIO_MUX(REG_PINMUX4, GENMASK(7, 4)),
	tmpv7700_GPIO_MUX(REG_PINMUX4, GENMASK(11, 8)),
	tmpv7700_GPIO_MUX(REG_PINMUX4, GENMASK(15, 12)),
	tmpv7700_GPIO_MUX(REG_PINMUX4, GENMASK(19, 16)),
	tmpv7700_GPIO_MUX(REG_PINMUX4, GENMASK(23, 20)),
	tmpv7700_GPIO_MUX(REG_PINMUX4, GENMASK(27, 24)),
	tmpv7700_GPIO_MUX(REG_PINMUX4, GENMASK(31, 28)),
	tmpv7700_GPIO_MUX(REG_PINMUX5, GENMASK(3, 0)),
	tmpv7700_GPIO_MUX(REG_PINMUX5, GENMASK(7, 4)),
	tmpv7700_GPIO_MUX(REG_PINMUX5, GENMASK(11, 8)),
	tmpv7700_GPIO_MUX(REG_PINMUX5, GENMASK(15, 12)),
	tmpv7700_GPIO_MUX(REG_PINMUX5, GENMASK(19, 16)),
	tmpv7700_GPIO_MUX(REG_PINMUX5, GENMASK(23, 20)),
	tmpv7700_GPIO_MUX(REG_PINMUX5, GENMASK(27, 24)),
	tmpv7700_GPIO_MUX(REG_PINMUX5, GENMASK(31, 28)),
};

static void tmpv7700_pinctrl_unlock(void __iomem *base)
{
	writel(1, base + REG_KEY_CTRL);
	writel(tmpv7700_MAGIC_NUM, base + REG_KEY_CMD);
}

/* chip dependent data */
static const struct visconti_pinctrl_devdata tmpv7700_pinctrl_data = {
	.pins = pins_tmpv7700,
	.nr_pins = ARRAY_SIZE(pins_tmpv7700),
	.groups = groups_tmpv7700,
	.nr_groups = ARRAY_SIZE(groups_tmpv7700),
	.functions = functions_tmpv7700,
	.nr_functions = ARRAY_SIZE(functions_tmpv7700),
	.gpio_mux = gpio_mux_tmpv7700,
	.unlock = tmpv7700_pinctrl_unlock,
};

static int tmpv7700_pinctrl_probe(struct platform_device *pdev)
{
	return visconti_pinctrl_probe(pdev, &tmpv7700_pinctrl_data);
}

static const struct of_device_id tmpv7700_pctrl_of_match[] = {
	{ .compatible = "toshiba,tmpv7708-pinctrl", },
	{},
};

static struct platform_driver tmpv7700_pinctrl_driver = {
	.probe = tmpv7700_pinctrl_probe,
	.driver = {
		.name = "tmpv7700-pinctrl",
		.of_match_table = tmpv7700_pctrl_of_match,
	},
};

static int __init tmpv7700_pinctrl_init(void)
{
	return platform_driver_register(&tmpv7700_pinctrl_driver);
}
arch_initcall(tmpv7700_pinctrl_init);
