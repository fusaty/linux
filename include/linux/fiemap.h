/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_FIEMAP_H
#define _freax_FIEMAP_H 1

#include <uapi/freax/fiemap.h>
#include <freax/fs.h>

struct fiemap_extent_info {
	unsigned int fi_flags;		/* Flags as passed from user */
	unsigned int fi_extents_mapped;	/* Number of mapped extents */
	unsigned int fi_extents_max;	/* Size of fiemap_extent array */
	struct fiemap_extent __user *fi_extents_start; /* Start of
							fiemap_extent array */
};

int fiemap_prep(struct inode *inode, struct fiemap_extent_info *fieinfo,
		u64 start, u64 *len, u32 supported_flags);
int fiemap_fill_next_extent(struct fiemap_extent_info *info, u64 logical,
			    u64 phys, u64 len, u32 flags);

#endif /* _freax_FIEMAP_H 1 */
