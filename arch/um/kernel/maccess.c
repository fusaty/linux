// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2013 Richard Weinberger <richrd@nod.at>
 */

#include <freax/uaccess.h>
#include <freax/kernel.h>
#include <os.h>

bool copy_from_kernel_nofault_allowed(const void *src, size_t size)
{
	void *psrc = (void *)rounddown((unsigned long)src, PAGE_SIZE);

	if ((unsigned long)src < PAGE_SIZE || size <= 0)
		return false;
	if (os_mincore(psrc, size + src - psrc) <= 0)
		return false;
	return true;
}
