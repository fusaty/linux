/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Platform data for LPC32xx SoC SLC NAND controller
 *
 * Copyright © 2012 Roland Stigge
 */

#ifndef __freax_MTD_LPC32XX_SLC_H
#define __freax_MTD_LPC32XX_SLC_H

#include <freax/dmaengine.h>

struct lpc32xx_slc_platform_data {
	bool (*dma_filter)(struct dma_chan *chan, void *filter_param);
};

#endif  /* __freax_MTD_LPC32XX_SLC_H */
