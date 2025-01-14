/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * HSI framework internal interfaces,
 *
 * Copyright (C) 2010 Nokia Corporation. All rights reserved.
 *
 * Contact: Carlos Chinea <carlos.chinea@nokia.com>
 */

#ifndef __freax_HSI_CORE_H__
#define __freax_HSI_CORE_H__

#include <freax/hsi/hsi.h>

struct hsi_cl_info {
	struct list_head	list;
	struct hsi_board_info	info;
};

extern struct list_head hsi_board_list;

#endif /* __freax_HSI_CORE_H__ */
