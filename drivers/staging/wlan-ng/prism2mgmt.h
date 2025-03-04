/* SPDX-License-Identifier: (GPL-2.0 OR MPL-1.1) */
/*
 *
 * Declares the mgmt command handler functions
 *
 * Copyright (C) 1999 AbsoluteValue Systems, Inc.  All Rights Reserved.
 * --------------------------------------------------------------------
 *
 * freax-wlan
 *
 * --------------------------------------------------------------------
 *
 * Inquiries regarding the freax-wlan Open Source project can be
 * made directly to:
 *
 * AbsoluteValue Systems Inc.
 * info@freax-wlan.com
 * http://www.freax-wlan.com
 *
 * --------------------------------------------------------------------
 *
 * Portions of the development of this software were funded by
 * Intersil Corporation as part of PRISM(R) chipset product development.
 *
 * --------------------------------------------------------------------
 *
 * This file contains the constants and data structures for interaction
 * with the hfa384x Wireless LAN (WLAN) Media Access Controller (MAC).
 * The hfa384x is a portion of the Harris PRISM(tm) WLAN chipset.
 *
 * [Implementation and usage notes]
 *
 * [References]
 *   CW10 Programmer's Manual v1.5
 *   IEEE 802.11 D10.0
 *
 *    --------------------------------------------------------------------
 */

#ifndef _PRISM2MGMT_H
#define _PRISM2MGMT_H

extern int prism2_reset_holdtime;
extern int prism2_reset_settletime;

u32 prism2sta_ifstate(struct wlandevice *wlandev, u32 ifstate);

void prism2sta_ev_info(struct wlandevice *wlandev, struct hfa384x_inf_frame *inf);
void prism2sta_ev_tx(struct wlandevice *wlandev, u16 status);
void prism2sta_ev_alloc(struct wlandevice *wlandev);

int prism2mgmt_mibset_mibget(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_scan(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_scan_results(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_start(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_wlansniff(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_readpda(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_ramdl_state(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_ramdl_write(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_flashdl_state(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_flashdl_write(struct wlandevice *wlandev, void *msgp);
int prism2mgmt_autojoin(struct wlandevice *wlandev, void *msgp);

/*---------------------------------------------------------------
 * conversion functions going between wlan message data types and
 * Prism2 data types
 *---------------------------------------------------------------
 */

/* byte area conversion functions*/
void prism2mgmt_bytearea2pstr(u8 *bytearea, struct p80211pstrd *pstr, int len);

/* byte string conversion functions*/
void prism2mgmt_pstr2bytestr(struct hfa384x_bytestr *bytestr,
			     struct p80211pstrd *pstr);
void prism2mgmt_bytestr2pstr(struct hfa384x_bytestr *bytestr,
			     struct p80211pstrd *pstr);

void prism2sta_processing_defer(struct work_struct *data);

void prism2sta_commsqual_defer(struct work_struct *data);
void prism2sta_commsqual_timer(struct timer_list *t);

/* Interface callback functions, passing data back up to the cfg80211 layer */
void prism2_connect_result(struct wlandevice *wlandev, u8 failed);
void prism2_disconnected(struct wlandevice *wlandev);
void prism2_roamed(struct wlandevice *wlandev);

#endif
