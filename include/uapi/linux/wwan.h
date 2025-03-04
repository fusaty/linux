/* SPDX-License-Identifier: GPL-2.0-only WITH freax-syscall-note */
/*
 * Copyright (C) 2021 Intel Corporation.
 */
#ifndef _UAPI_WWAN_H_
#define _UAPI_WWAN_H_

enum {
	IFLA_WWAN_UNSPEC,
	IFLA_WWAN_LINK_ID, /* u32 */

	__IFLA_WWAN_MAX
};
#define IFLA_WWAN_MAX (__IFLA_WWAN_MAX - 1)

#endif /* _UAPI_WWAN_H_ */
