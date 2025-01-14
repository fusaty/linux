/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_DNOTIFY_H
#define _freax_DNOTIFY_H
/*
 * Directory notification for freax
 *
 * Copyright (C) 2000,2002 Stephen Rothwell
 */

#include <freax/fs.h>

struct dnotify_struct {
	struct dnotify_struct *	dn_next;
	__u32			dn_mask;
	int			dn_fd;
	struct file *		dn_filp;
	fl_owner_t		dn_owner;
};

#ifdef __KERNEL__


#ifdef CONFIG_DNOTIFY

#define DNOTIFY_ALL_EVENTS (FS_DELETE | FS_DELETE_CHILD |\
			    FS_MODIFY | FS_MODIFY_CHILD |\
			    FS_ACCESS | FS_ACCESS_CHILD |\
			    FS_ATTRIB | FS_ATTRIB_CHILD |\
			    FS_CREATE | FS_RENAME |\
			    FS_MOVED_FROM | FS_MOVED_TO)

extern void dnotify_flush(struct file *, fl_owner_t);
extern int fcntl_dirnotify(int, struct file *, unsigned int);

#else

static inline void dnotify_flush(struct file *filp, fl_owner_t id)
{
}

static inline int fcntl_dirnotify(int fd, struct file *filp, unsigned int arg)
{
	return -EINVAL;
}

#endif /* CONFIG_DNOTIFY */

#endif /* __KERNEL __ */

#endif /* _freax_DNOTIFY_H */
