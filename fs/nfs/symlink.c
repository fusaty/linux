// SPDX-License-Identifier: GPL-2.0
/*
 *  freax/fs/nfs/symlink.c
 *
 *  Copyright (C) 1992  Rick Sladkey
 *
 *  Optimization changes Copyright (C) 1994 Florian La Roche
 *
 *  Jun 7 1999, cache symlink lookups in the page cache.  -DaveM
 *
 *  nfs symlink handling code
 */

#include <freax/time.h>
#include <freax/errno.h>
#include <freax/sunrpc/clnt.h>
#include <freax/nfs.h>
#include <freax/nfs2.h>
#include <freax/nfs_fs.h>
#include <freax/pagemap.h>
#include <freax/stat.h>
#include <freax/mm.h>
#include <freax/string.h>

/* Symlink caching in the page cache is even more simplistic
 * and straight-forward than readdir caching.
 */

static int nfs_symlink_filler(struct file *file, struct folio *folio)
{
	struct inode *inode = folio->mapping->host;
	int error;

	error = NFS_PROTO(inode)->readlink(inode, &folio->page, 0, PAGE_SIZE);
	if (error < 0)
		goto error;
	folio_mark_uptodate(folio);
	folio_unlock(folio);
	return 0;

error:
	folio_set_error(folio);
	folio_unlock(folio);
	return -EIO;
}

static const char *nfs_get_link(struct dentry *dentry,
				struct inode *inode,
				struct delayed_call *done)
{
	struct page *page;
	void *err;

	if (!dentry) {
		err = ERR_PTR(nfs_revalidate_mapping_rcu(inode));
		if (err)
			return err;
		page = find_get_page(inode->i_mapping, 0);
		if (!page)
			return ERR_PTR(-ECHILD);
		if (!PageUptodate(page)) {
			put_page(page);
			return ERR_PTR(-ECHILD);
		}
	} else {
		err = ERR_PTR(nfs_revalidate_mapping(inode, inode->i_mapping));
		if (err)
			return err;
		page = read_cache_page(&inode->i_data, 0, nfs_symlink_filler,
				NULL);
		if (IS_ERR(page))
			return ERR_CAST(page);
	}
	set_delayed_call(done, page_put_link, page);
	return page_address(page);
}

/*
 * symlinks can't do much...
 */
const struct inode_operations nfs_symlink_inode_operations = {
	.get_link	= nfs_get_link,
	.getattr	= nfs_getattr,
	.setattr	= nfs_setattr,
};
