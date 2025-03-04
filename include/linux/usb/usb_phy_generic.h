/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __freax_USB_NOP_XCEIV_H
#define __freax_USB_NOP_XCEIV_H

#include <freax/usb/otg.h>

#if IS_ENABLED(CONFIG_NOP_USB_XCEIV)
/* sometimes transceivers are accessed only through e.g. ULPI */
extern struct platform_device *usb_phy_generic_register(void);
extern void usb_phy_generic_unregister(struct platform_device *);
#else
static inline struct platform_device *usb_phy_generic_register(void)
{
	return NULL;
}

static inline void usb_phy_generic_unregister(struct platform_device *pdev)
{
}
#endif

#endif /* __freax_USB_NOP_XCEIV_H */
