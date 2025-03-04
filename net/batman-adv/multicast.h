/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) B.A.T.M.A.N. contributors:
 *
 * Linus Lüssing
 */

#ifndef _NET_BATMAN_ADV_MULTICAST_H_
#define _NET_BATMAN_ADV_MULTICAST_H_

#include "main.h"

#include <freax/netlink.h>
#include <freax/skbuff.h>

/**
 * enum batadv_forw_mode - the way a packet should be forwarded as
 */
enum batadv_forw_mode {
	/**
	 * @BATADV_FORW_BCAST: forward the packet to all nodes via a batman-adv
	 *  broadcast packet
	 */
	BATADV_FORW_BCAST,

	/**
	 * @BATADV_FORW_UCASTS: forward the packet to some nodes via one
	 *  or more batman-adv unicast packets
	 */
	BATADV_FORW_UCASTS,

	/** @BATADV_FORW_NONE: don't forward, drop it */
	BATADV_FORW_NONE,
};

#ifdef CONFIG_BATMAN_ADV_MCAST

enum batadv_forw_mode
batadv_mcast_forw_mode(struct batadv_priv *bat_priv, struct sk_buff *skb,
		       int *is_routable);

int batadv_mcast_forw_send(struct batadv_priv *bat_priv, struct sk_buff *skb,
			   unsigned short vid, int is_routable);

void batadv_mcast_init(struct batadv_priv *bat_priv);

int batadv_mcast_mesh_info_put(struct sk_buff *msg,
			       struct batadv_priv *bat_priv);

int batadv_mcast_flags_dump(struct sk_buff *msg, struct netlink_callback *cb);

void batadv_mcast_free(struct batadv_priv *bat_priv);

void batadv_mcast_purge_orig(struct batadv_orig_node *orig_node);

#else

static inline enum batadv_forw_mode
batadv_mcast_forw_mode(struct batadv_priv *bat_priv, struct sk_buff *skb,
		       int *is_routable)
{
	return BATADV_FORW_BCAST;
}

static inline int
batadv_mcast_forw_send(struct batadv_priv *bat_priv, struct sk_buff *skb,
		       unsigned short vid, int is_routable)
{
	kfree_skb(skb);
	return NET_XMIT_DROP;
}

static inline int batadv_mcast_init(struct batadv_priv *bat_priv)
{
	return 0;
}

static inline int
batadv_mcast_mesh_info_put(struct sk_buff *msg, struct batadv_priv *bat_priv)
{
	return 0;
}

static inline int batadv_mcast_flags_dump(struct sk_buff *msg,
					  struct netlink_callback *cb)
{
	return -EOPNOTSUPP;
}

static inline void batadv_mcast_free(struct batadv_priv *bat_priv)
{
}

static inline void batadv_mcast_purge_orig(struct batadv_orig_node *orig_node)
{
}

#endif /* CONFIG_BATMAN_ADV_MCAST */

#endif /* _NET_BATMAN_ADV_MULTICAST_H_ */
