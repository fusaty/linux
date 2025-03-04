/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * Purpose: Provide functions to setup NIC operation mode
 *
 * Author: Tevin Chen
 *
 * Date: May 21, 1996
 *
 */

#ifndef __CARD_H__
#define __CARD_H__

#include <freax/types.h>
#include <freax/nl80211.h>

/*
 * Loopback mode
 *
 * LOBYTE is MAC LB mode, HIBYTE is MII LB mode
 */
#define CARD_LB_NONE            MAKEWORD(MAC_LB_NONE, 0)
/* PHY must ISO, avoid MAC loopback packet go out */
#define CARD_LB_MAC             MAKEWORD(MAC_LB_INTERNAL, 0)
#define CARD_LB_PHY             MAKEWORD(MAC_LB_EXT, 0)

#define DEFAULT_MSDU_LIFETIME           512  /* ms */
#define DEFAULT_MSDU_LIFETIME_RES_64us  8000 /* 64us */

#define DEFAULT_MGN_LIFETIME            8    /* ms */
#define DEFAULT_MGN_LIFETIME_RES_64us   125  /* 64us */

#define CB_MAX_CHANNEL_24G      14
#define CB_MAX_CHANNEL_5G       42
#define CB_MAX_CHANNEL          (CB_MAX_CHANNEL_24G + CB_MAX_CHANNEL_5G)

struct vnt_private;

void card_set_rspinf(struct vnt_private *priv, u8 bb_type);
void CARDvUpdateBasicTopRate(struct vnt_private *priv);
bool CARDbIsOFDMinBasicRate(struct vnt_private *priv);
void CARDvSetFirstNextTBTT(struct vnt_private *priv,
			   unsigned short wBeaconInterval);
void CARDvUpdateNextTBTT(struct vnt_private *priv, u64 qwTSF,
			 unsigned short wBeaconInterval);
u64 vt6655_get_current_tsf(struct vnt_private *priv);
u64 CARDqGetNextTBTT(u64 qwTSF, unsigned short wBeaconInterval);
u64 CARDqGetTSFOffset(unsigned char rx_rate, u64 qwTSF1, u64 qwTSF2);
unsigned char card_get_pkt_type(struct vnt_private *priv);
void CARDvSafeResetTx(struct vnt_private *priv);
void CARDvSafeResetRx(struct vnt_private *priv);
void CARDbRadioPowerOff(struct vnt_private *priv);
bool card_set_phy_parameter(struct vnt_private *priv, u8 bb_type);
bool card_update_tsf(struct vnt_private *priv, unsigned char rx_rate,
		    u64 qwBSSTimestamp);
bool CARDbSetBeaconPeriod(struct vnt_private *priv,
			  unsigned short wBeaconInterval);

#endif /* __CARD_H__ */
