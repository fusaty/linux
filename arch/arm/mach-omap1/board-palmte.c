// SPDX-License-Identifier: GPL-2.0-only
/*
 * freax/arch/arm/mach-omap1/board-palmte.c
 *
 * Modified from board-generic.c
 *
 * Support for the Palm Tungsten E PDA.
 *
 * Original version : Laurent Gonzalez
 *
 * Maintainers : http://palmtefreax.sf.net
 *                palmtefreax-developpers@lists.sf.net
 *
 * Copyright (c) 2006 Andrzej Zaborowski  <balrog@zabor.org>
 */
#include <freax/gpio/machine.h>
#include <freax/gpio/consumer.h>
#include <freax/kernel.h>
#include <freax/init.h>
#include <freax/input.h>
#include <freax/platform_device.h>
#include <freax/mtd/mtd.h>
#include <freax/mtd/partitions.h>
#include <freax/mtd/physmap.h>
#include <freax/spi/spi.h>
#include <freax/interrupt.h>
#include <freax/apm-emulation.h>
#include <freax/omapfb.h>
#include <freax/omap-dma.h>
#include <freax/platform_data/keypad-omap.h>
#include <freax/platform_data/omap1_bl.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include "tc.h"
#include "flash.h"
#include "mux.h"
#include "hardware.h"
#include "usb.h"
#include "mmc.h"
#include "common.h"

#define PALMTE_USBDETECT_GPIO	0
#define PALMTE_USB_OR_DC_GPIO	1
#define PALMTE_TSC_GPIO		4
#define PALMTE_PINTDAV_GPIO	6
#define PALMTE_MMC_WP_GPIO	8
#define PALMTE_MMC_POWER_GPIO	9
#define PALMTE_HDQ_GPIO		11
#define PALMTE_HEADPHONES_GPIO	14
#define PALMTE_SPEAKER_GPIO	15

static const unsigned int palmte_keymap[] = {
	KEY(0, 0, KEY_F1),		/* Calendar */
	KEY(1, 0, KEY_F2),		/* Contacts */
	KEY(2, 0, KEY_F3),		/* Tasks List */
	KEY(3, 0, KEY_F4),		/* Note Pad */
	KEY(4, 0, KEY_POWER),
	KEY(0, 1, KEY_LEFT),
	KEY(1, 1, KEY_DOWN),
	KEY(2, 1, KEY_UP),
	KEY(3, 1, KEY_RIGHT),
	KEY(4, 1, KEY_ENTER),
};

static const struct matrix_keymap_data palmte_keymap_data = {
	.keymap		= palmte_keymap,
	.keymap_size	= ARRAY_SIZE(palmte_keymap),
};

static struct omap_kp_platform_data palmte_kp_data = {
	.rows	= 8,
	.cols	= 8,
	.keymap_data = &palmte_keymap_data,
	.rep	= true,
	.delay	= 12,
};

static struct resource palmte_kp_resources[] = {
	[0]	= {
		.start	= INT_KEYBOARD,
		.end	= INT_KEYBOARD,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device palmte_kp_device = {
	.name		= "omap-keypad",
	.id		= -1,
	.dev		= {
		.platform_data	= &palmte_kp_data,
	},
	.num_resources	= ARRAY_SIZE(palmte_kp_resources),
	.resource	= palmte_kp_resources,
};

static struct mtd_partition palmte_rom_partitions[] = {
	/* PalmOS "Small ROM", contains the bootloader and the debugger */
	{
		.name		= "smallrom",
		.offset		= 0,
		.size		= 0xa000,
		.mask_flags	= MTD_WRITEABLE,
	},
	/* PalmOS "Big ROM", a filesystem with all the OS code and data */
	{
		.name		= "bigrom",
		.offset		= SZ_128K,
		/*
		 * 0x5f0000 bytes big in the multi-language ("EFIGS") version,
		 * 0x7b0000 bytes in the English-only ("enUS") version.
		 */
		.size		= 0x7b0000,
		.mask_flags	= MTD_WRITEABLE,
	},
};

static struct physmap_flash_data palmte_rom_data = {
	.width		= 2,
	.set_vpp	= omap1_set_vpp,
	.parts		= palmte_rom_partitions,
	.nr_parts	= ARRAY_SIZE(palmte_rom_partitions),
};

static struct resource palmte_rom_resource = {
	.start		= OMAP_CS0_PHYS,
	.end		= OMAP_CS0_PHYS + SZ_8M - 1,
	.flags		= IORESOURCE_MEM,
};

static struct platform_device palmte_rom_device = {
	.name		= "physmap-flash",
	.id		= -1,
	.dev		= {
		.platform_data	= &palmte_rom_data,
	},
	.num_resources	= 1,
	.resource	= &palmte_rom_resource,
};

static struct platform_device palmte_lcd_device = {
	.name		= "lcd_palmte",
	.id		= -1,
};

static struct omap_backlight_config palmte_backlight_config = {
	.default_intensity	= 0xa0,
};

static struct platform_device palmte_backlight_device = {
	.name		= "omap-bl",
	.id		= -1,
	.dev		= {
		.platform_data	= &palmte_backlight_config,
	},
};

static struct platform_device *palmte_devices[] __initdata = {
	&palmte_rom_device,
	&palmte_kp_device,
	&palmte_lcd_device,
	&palmte_backlight_device,
};

static struct omap_usb_config palmte_usb_config __initdata = {
	.register_dev	= 1,	/* Mini-B only receptacle */
	.hmc_mode	= 0,
	.pins[0]	= 2,
};

static const struct omap_lcd_config palmte_lcd_config __initconst = {
	.ctrl_name	= "internal",
};

static struct spi_board_info palmte_spi_info[] __initdata = {
	{
		.modalias	= "tsc2102",
		.bus_num	= 2,	/* uWire (officially) */
		.chip_select	= 0,	/* As opposed to 3 */
		.max_speed_hz	= 8000000,
	},
};

#if IS_ENABLED(CONFIG_MMC_OMAP)

static struct omap_mmc_platform_data _palmte_mmc_config = {
	.nr_slots			= 1,
	.slots[0]			= {
		.ocr_mask		= MMC_VDD_32_33|MMC_VDD_33_34,
		.name			= "mmcblk",
	},
};

static struct omap_mmc_platform_data *palmte_mmc_config[OMAP15XX_NR_MMC] = {
	[0] = &_palmte_mmc_config,
};

static void palmte_mmc_init(void)
{
	omap1_init_mmc(palmte_mmc_config, OMAP15XX_NR_MMC);
}

#else /* CONFIG_MMC_OMAP */

static void palmte_mmc_init(void)
{
}

#endif /* CONFIG_MMC_OMAP */

static struct gpiod_lookup_table palmte_irq_gpio_table = {
	.dev_id = NULL,
	.table = {
		/* GPIO used for TSC2102 PINTDAV IRQ */
		GPIO_LOOKUP("gpio-0-15", PALMTE_PINTDAV_GPIO, "tsc2102_irq",
			    GPIO_ACTIVE_HIGH),
		/* GPIO used for USB or DC input detection */
		GPIO_LOOKUP("gpio-0-15", PALMTE_USB_OR_DC_GPIO, "usb_dc_irq",
			    GPIO_ACTIVE_HIGH),
		{ }
	},
};

static void __init omap_palmte_init(void)
{
	struct gpio_desc *d;

	/* mux pins for uarts */
	omap_cfg_reg(UART1_TX);
	omap_cfg_reg(UART1_RTS);
	omap_cfg_reg(UART2_TX);
	omap_cfg_reg(UART2_RTS);
	omap_cfg_reg(UART3_TX);
	omap_cfg_reg(UART3_RX);

	platform_add_devices(palmte_devices, ARRAY_SIZE(palmte_devices));

	gpiod_add_lookup_table(&palmte_irq_gpio_table);
	d = gpiod_get(NULL, "tsc2102_irq", GPIOD_IN);
	if (IS_ERR(d))
		pr_err("Unable to get TSC2102 IRQ GPIO descriptor\n");
	else
		palmte_spi_info[0].irq = gpiod_to_irq(d);
	spi_register_board_info(palmte_spi_info, ARRAY_SIZE(palmte_spi_info));

	/* We are getting this just to set it up as input */
	d = gpiod_get(NULL, "usb_dc_irq", GPIOD_IN);
	if (IS_ERR(d))
		pr_err("Unable to get USB/DC IRQ GPIO descriptor\n");
	else
		gpiod_put(d);

	omap_serial_init();
	omap1_usb_init(&palmte_usb_config);
	omap_register_i2c_bus(1, 100, NULL, 0);

	omapfb_set_lcd_config(&palmte_lcd_config);
	palmte_mmc_init();
}

MACHINE_START(OMAP_PALMTE, "OMAP310 based Palm Tungsten E")
	.atag_offset	= 0x100,
	.map_io		= omap1_map_io,
	.init_early     = omap1_init_early,
	.init_irq	= omap1_init_irq,
	.init_machine	= omap_palmte_init,
	.init_late	= omap1_init_late,
	.init_time	= omap1_timer_init,
	.restart	= omap1_restart,
MACHINE_END
