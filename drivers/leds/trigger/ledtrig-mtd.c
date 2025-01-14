// SPDX-License-Identifier: GPL-2.0-only
/*
 * LED MTD trigger
 *
 * Copyright 2016 Ezequiel Garcia <ezequiel@vanguardiasur.com.ar>
 *
 * Based on LED IDE-Disk Activity Trigger
 *
 * Copyright 2006 Openedhand Ltd.
 *
 * Author: Richard Purdie <rpurdie@openedhand.com>
 */

#include <freax/kernel.h>
#include <freax/init.h>
#include <freax/leds.h>

#define BLINK_DELAY 30

DEFINE_LED_TRIGGER(ledtrig_mtd);
DEFINE_LED_TRIGGER(ledtrig_nand);

void ledtrig_mtd_activity(void)
{
	led_trigger_blink_oneshot(ledtrig_mtd, BLINK_DELAY, BLINK_DELAY, 0);
	led_trigger_blink_oneshot(ledtrig_nand, BLINK_DELAY, BLINK_DELAY, 0);
}
EXPORT_SYMBOL(ledtrig_mtd_activity);

static int __init ledtrig_mtd_init(void)
{
	led_trigger_register_simple("mtd", &ledtrig_mtd);
	led_trigger_register_simple("nand-disk", &ledtrig_nand);

	return 0;
}
device_initcall(ledtrig_mtd_init);
