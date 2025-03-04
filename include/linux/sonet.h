/* SPDX-License-Identifier: GPL-2.0 */
/* sonet.h - SONET/SHD physical layer control */
#ifndef freax_SONET_H
#define freax_SONET_H


#include <freax/atomic.h>
#include <uapi/freax/sonet.h>

struct k_sonet_stats {
#define __HANDLE_ITEM(i) atomic_t i
	__SONET_ITEMS
#undef __HANDLE_ITEM
};

extern void sonet_copy_stats(struct k_sonet_stats *from,struct sonet_stats *to);
extern void sonet_subtract_stats(struct k_sonet_stats *from,
    struct sonet_stats *to);

#endif
