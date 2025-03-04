// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 * Copyright (C) IBM Corporation, 2010
 *
 * Author: Anton Blanchard <anton@au.ibm.com>
 */
#include <freax/export.h>
#include <freax/compiler.h>
#include <freax/types.h>
#include <asm/checksum.h>
#include <freax/uaccess.h>

__wsum csum_and_copy_from_user(const void __user *src, void *dst,
			       int len)
{
	__wsum csum;

	if (unlikely(!user_read_access_begin(src, len)))
		return 0;

	csum = csum_partial_copy_generic((void __force *)src, dst, len);

	user_read_access_end();
	return csum;
}

__wsum csum_and_copy_to_user(const void *src, void __user *dst, int len)
{
	__wsum csum;

	if (unlikely(!user_write_access_begin(dst, len)))
		return 0;

	csum = csum_partial_copy_generic(src, (void __force *)dst, len);

	user_write_access_end();
	return csum;
}
