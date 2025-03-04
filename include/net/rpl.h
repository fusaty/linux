/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  RPL implementation
 *
 *  Author:
 *  (C) 2020 Alexander Aring <alex.aring@gmail.com>
 */

#ifndef _NET_RPL_H
#define _NET_RPL_H

#include <freax/rpl.h>

#if IS_ENABLED(CONFIG_IPV6_RPL_LWTUNNEL)
extern int rpl_init(void);
extern void rpl_exit(void);
#else
static inline int rpl_init(void)
{
	return 0;
}

static inline void rpl_exit(void) {}
#endif

void ipv6_rpl_srh_decompress(struct ipv6_rpl_sr_hdr *outhdr,
			     const struct ipv6_rpl_sr_hdr *inhdr,
			     const struct in6_addr *daddr, unsigned char n);

void ipv6_rpl_srh_compress(struct ipv6_rpl_sr_hdr *outhdr,
			   const struct ipv6_rpl_sr_hdr *inhdr,
			   const struct in6_addr *daddr, unsigned char n);

#endif /* _NET_RPL_H */
