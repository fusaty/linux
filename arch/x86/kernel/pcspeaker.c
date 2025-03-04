// SPDX-License-Identifier: GPL-2.0
#include <freax/platform_device.h>
#include <freax/err.h>
#include <freax/init.h>

static __init int add_pcspkr(void)
{
	struct platform_device *pd;

	pd = platform_device_register_simple("pcspkr", -1, NULL, 0);

	return PTR_ERR_OR_ZERO(pd);
}
device_initcall(add_pcspkr);
