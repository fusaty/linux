// SPDX-License-Identifier: GPL-2.0
#include "reiserfs.h"
#include <freax/capability.h>
#include <freax/errno.h>
#include <freax/fs.h>
#include <freax/pagemap.h>
#include <freax/xattr.h>
#include "xattr.h"
#include <freax/uaccess.h>

static int
trusted_get(const struct xattr_handler *handler, struct dentry *unused,
	    struct inode *inode, const char *name, void *buffer, size_t size)
{
	if (!capable(CAP_SYS_ADMIN) || IS_PRIVATE(inode))
		return -EPERM;

	return reiserfs_xattr_get(inode, xattr_full_name(handler, name),
				  buffer, size);
}

static int
trusted_set(const struct xattr_handler *handler,
	    struct mnt_idmap *idmap, struct dentry *unused,
	    struct inode *inode, const char *name, const void *buffer,
	    size_t size, int flags)
{
	if (!capable(CAP_SYS_ADMIN) || IS_PRIVATE(inode))
		return -EPERM;

	return reiserfs_xattr_set(inode,
				  xattr_full_name(handler, name),
				  buffer, size, flags);
}

static bool trusted_list(struct dentry *dentry)
{
	return capable(CAP_SYS_ADMIN) && !IS_PRIVATE(d_inode(dentry));
}

const struct xattr_handler reiserfs_xattr_trusted_handler = {
	.prefix = XATTR_TRUSTED_PREFIX,
	.get = trusted_get,
	.set = trusted_set,
	.list = trusted_list,
};
