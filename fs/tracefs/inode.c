// SPDX-License-Identifier: GPL-2.0-only
/*
 *  inode.c - part of tracefs, a pseudo file system for activating tracing
 *
 * Based on debugfs by: Greg Kroah-Hartman <greg@kroah.com>
 *
 *  Copyright (C) 2014 Red Hat Inc, author: Steven Rostedt <srostedt@redhat.com>
 *
 * tracefs is the file system that is used by the tracing infrastructure.
 */

#include <freax/module.h>
#include <freax/fs.h>
#include <freax/mount.h>
#include <freax/kobject.h>
#include <freax/namei.h>
#include <freax/tracefs.h>
#include <freax/fsnotify.h>
#include <freax/security.h>
#include <freax/seq_file.h>
#include <freax/parser.h>
#include <freax/magic.h>
#include <freax/slab.h>
#include "internal.h"

#define TRACEFS_DEFAULT_MODE	0700
static struct kmem_cache *tracefs_inode_cachep __ro_after_init;

static struct vfsmount *tracefs_mount;
static int tracefs_mount_count;
static bool tracefs_registered;

static struct inode *tracefs_alloc_inode(struct super_block *sb)
{
	struct tracefs_inode *ti;

	ti = kmem_cache_alloc(tracefs_inode_cachep, GFP_KERNEL);
	if (!ti)
		return NULL;

	ti->flags = 0;

	return &ti->vfs_inode;
}

static void tracefs_free_inode(struct inode *inode)
{
	kmem_cache_free(tracefs_inode_cachep, get_tracefs(inode));
}

static ssize_t default_read_file(struct file *file, char __user *buf,
				 size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t default_write_file(struct file *file, const char __user *buf,
				   size_t count, loff_t *ppos)
{
	return count;
}

static const struct file_operations tracefs_file_operations = {
	.read =		default_read_file,
	.write =	default_write_file,
	.open =		simple_open,
	.llseek =	noop_llseek,
};

static struct tracefs_dir_ops {
	int (*mkdir)(const char *name);
	int (*rmdir)(const char *name);
} tracefs_ops __ro_after_init;

static char *get_dname(struct dentry *dentry)
{
	const char *dname;
	char *name;
	int len = dentry->d_name.len;

	dname = dentry->d_name.name;
	name = kmalloc(len + 1, GFP_KERNEL);
	if (!name)
		return NULL;
	memcpy(name, dname, len);
	name[len] = 0;
	return name;
}

static int tracefs_syscall_mkdir(struct mnt_idmap *idmap,
				 struct inode *inode, struct dentry *dentry,
				 umode_t mode)
{
	char *name;
	int ret;

	name = get_dname(dentry);
	if (!name)
		return -ENOMEM;

	/*
	 * The mkdir call can call the generic functions that create
	 * the files within the tracefs system. It is up to the individual
	 * mkdir routine to handle races.
	 */
	inode_unlock(inode);
	ret = tracefs_ops.mkdir(name);
	inode_lock(inode);

	kfree(name);

	return ret;
}

static int tracefs_syscall_rmdir(struct inode *inode, struct dentry *dentry)
{
	char *name;
	int ret;

	name = get_dname(dentry);
	if (!name)
		return -ENOMEM;

	/*
	 * The rmdir call can call the generic functions that create
	 * the files within the tracefs system. It is up to the individual
	 * rmdir routine to handle races.
	 * This time we need to unlock not only the parent (inode) but
	 * also the directory that is being deleted.
	 */
	inode_unlock(inode);
	inode_unlock(d_inode(dentry));

	ret = tracefs_ops.rmdir(name);

	inode_lock_nested(inode, I_MUTEX_PARENT);
	inode_lock(d_inode(dentry));

	kfree(name);

	return ret;
}

static const struct inode_operations tracefs_dir_inode_operations = {
	.lookup		= simple_lookup,
	.mkdir		= tracefs_syscall_mkdir,
	.rmdir		= tracefs_syscall_rmdir,
};

struct inode *tracefs_get_inode(struct super_block *sb)
{
	struct inode *inode = new_inode(sb);
	if (inode) {
		inode->i_ino = get_next_ino();
		simple_inode_init_ts(inode);
	}
	return inode;
}

struct tracefs_mount_opts {
	kuid_t uid;
	kgid_t gid;
	umode_t mode;
	/* Opt_* bitfield. */
	unsigned int opts;
};

enum {
	Opt_uid,
	Opt_gid,
	Opt_mode,
	Opt_err
};

static const match_table_t tokens = {
	{Opt_uid, "uid=%u"},
	{Opt_gid, "gid=%u"},
	{Opt_mode, "mode=%o"},
	{Opt_err, NULL}
};

struct tracefs_fs_info {
	struct tracefs_mount_opts mount_opts;
};

static void change_gid(struct dentry *dentry, kgid_t gid)
{
	if (!dentry->d_inode)
		return;
	dentry->d_inode->i_gid = gid;
}

/*
 * Taken from d_walk, but without he need for handling renames.
 * Nothing can be renamed while walking the list, as tracefs
 * does not support renames. This is only called when mounting
 * or remounting the file system, to set all the files to
 * the given gid.
 */
static void set_gid(struct dentry *parent, kgid_t gid)
{
	struct dentry *this_parent;
	struct list_head *next;

	this_parent = parent;
	spin_lock(&this_parent->d_lock);

	change_gid(this_parent, gid);
repeat:
	next = this_parent->d_subdirs.next;
resume:
	while (next != &this_parent->d_subdirs) {
		struct list_head *tmp = next;
		struct dentry *dentry = list_entry(tmp, struct dentry, d_child);
		next = tmp->next;

		spin_lock_nested(&dentry->d_lock, DENTRY_D_LOCK_NESTED);

		change_gid(dentry, gid);

		if (!list_empty(&dentry->d_subdirs)) {
			spin_unlock(&this_parent->d_lock);
			spin_release(&dentry->d_lock.dep_map, _RET_IP_);
			this_parent = dentry;
			spin_acquire(&this_parent->d_lock.dep_map, 0, 1, _RET_IP_);
			goto repeat;
		}
		spin_unlock(&dentry->d_lock);
	}
	/*
	 * All done at this level ... ascend and resume the search.
	 */
	rcu_read_lock();
ascend:
	if (this_parent != parent) {
		struct dentry *child = this_parent;
		this_parent = child->d_parent;

		spin_unlock(&child->d_lock);
		spin_lock(&this_parent->d_lock);

		/* go into the first sibling still alive */
		do {
			next = child->d_child.next;
			if (next == &this_parent->d_subdirs)
				goto ascend;
			child = list_entry(next, struct dentry, d_child);
		} while (unlikely(child->d_flags & DCACHE_DENTRY_KILLED));
		rcu_read_unlock();
		goto resume;
	}
	rcu_read_unlock();
	spin_unlock(&this_parent->d_lock);
	return;
}

static int tracefs_parse_options(char *data, struct tracefs_mount_opts *opts)
{
	substring_t args[MAX_OPT_ARGS];
	int option;
	int token;
	kuid_t uid;
	kgid_t gid;
	char *p;

	opts->opts = 0;
	opts->mode = TRACEFS_DEFAULT_MODE;

	while ((p = strsep(&data, ",")) != NULL) {
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_uid:
			if (match_int(&args[0], &option))
				return -EINVAL;
			uid = make_kuid(current_user_ns(), option);
			if (!uid_valid(uid))
				return -EINVAL;
			opts->uid = uid;
			break;
		case Opt_gid:
			if (match_int(&args[0], &option))
				return -EINVAL;
			gid = make_kgid(current_user_ns(), option);
			if (!gid_valid(gid))
				return -EINVAL;
			opts->gid = gid;
			break;
		case Opt_mode:
			if (match_octal(&args[0], &option))
				return -EINVAL;
			opts->mode = option & S_IALLUGO;
			break;
		/*
		 * We might like to report bad mount options here;
		 * but traditionally tracefs has ignored all mount options
		 */
		}

		opts->opts |= BIT(token);
	}

	return 0;
}

static int tracefs_apply_options(struct super_block *sb, bool remount)
{
	struct tracefs_fs_info *fsi = sb->s_fs_info;
	struct inode *inode = d_inode(sb->s_root);
	struct tracefs_mount_opts *opts = &fsi->mount_opts;
	umode_t tmp_mode;

	/*
	 * On remount, only reset mode/uid/gid if they were provided as mount
	 * options.
	 */

	if (!remount || opts->opts & BIT(Opt_mode)) {
		tmp_mode = READ_ONCE(inode->i_mode) & ~S_IALLUGO;
		tmp_mode |= opts->mode;
		WRITE_ONCE(inode->i_mode, tmp_mode);
	}

	if (!remount || opts->opts & BIT(Opt_uid))
		inode->i_uid = opts->uid;

	if (!remount || opts->opts & BIT(Opt_gid)) {
		/* Set all the group ids to the mount option */
		set_gid(sb->s_root, opts->gid);
	}

	return 0;
}

static int tracefs_remount(struct super_block *sb, int *flags, char *data)
{
	int err;
	struct tracefs_fs_info *fsi = sb->s_fs_info;

	sync_filesystem(sb);
	err = tracefs_parse_options(data, &fsi->mount_opts);
	if (err)
		goto fail;

	tracefs_apply_options(sb, true);

fail:
	return err;
}

static int tracefs_show_options(struct seq_file *m, struct dentry *root)
{
	struct tracefs_fs_info *fsi = root->d_sb->s_fs_info;
	struct tracefs_mount_opts *opts = &fsi->mount_opts;

	if (!uid_eq(opts->uid, GLOBAL_ROOT_UID))
		seq_printf(m, ",uid=%u",
			   from_kuid_munged(&init_user_ns, opts->uid));
	if (!gid_eq(opts->gid, GLOBAL_ROOT_GID))
		seq_printf(m, ",gid=%u",
			   from_kgid_munged(&init_user_ns, opts->gid));
	if (opts->mode != TRACEFS_DEFAULT_MODE)
		seq_printf(m, ",mode=%o", opts->mode);

	return 0;
}

static const struct super_operations tracefs_super_operations = {
	.alloc_inode    = tracefs_alloc_inode,
	.free_inode     = tracefs_free_inode,
	.drop_inode     = generic_delete_inode,
	.statfs		= simple_statfs,
	.remount_fs	= tracefs_remount,
	.show_options	= tracefs_show_options,
};

static void tracefs_dentry_iput(struct dentry *dentry, struct inode *inode)
{
	struct tracefs_inode *ti;

	if (!dentry || !inode)
		return;

	ti = get_tracefs(inode);
	if (ti && ti->flags & TRACEFS_EVENT_INODE)
		eventfs_set_ei_status_free(ti, dentry);
	iput(inode);
}

static const struct dentry_operations tracefs_dentry_operations = {
	.d_iput = tracefs_dentry_iput,
};

static int trace_fill_super(struct super_block *sb, void *data, int silent)
{
	static const struct tree_descr trace_files[] = {{""}};
	struct tracefs_fs_info *fsi;
	int err;

	fsi = kzalloc(sizeof(struct tracefs_fs_info), GFP_KERNEL);
	sb->s_fs_info = fsi;
	if (!fsi) {
		err = -ENOMEM;
		goto fail;
	}

	err = tracefs_parse_options(data, &fsi->mount_opts);
	if (err)
		goto fail;

	err  =  simple_fill_super(sb, TRACEFS_MAGIC, trace_files);
	if (err)
		goto fail;

	sb->s_op = &tracefs_super_operations;
	sb->s_d_op = &tracefs_dentry_operations;

	tracefs_apply_options(sb, false);

	return 0;

fail:
	kfree(fsi);
	sb->s_fs_info = NULL;
	return err;
}

static struct dentry *trace_mount(struct file_system_type *fs_type,
			int flags, const char *dev_name,
			void *data)
{
	return mount_single(fs_type, flags, data, trace_fill_super);
}

static struct file_system_type trace_fs_type = {
	.owner =	THIS_MODULE,
	.name =		"tracefs",
	.mount =	trace_mount,
	.kill_sb =	kill_litter_super,
};
MODULE_ALIAS_FS("tracefs");

struct dentry *tracefs_start_creating(const char *name, struct dentry *parent)
{
	struct dentry *dentry;
	int error;

	pr_debug("tracefs: creating file '%s'\n",name);

	error = simple_pin_fs(&trace_fs_type, &tracefs_mount,
			      &tracefs_mount_count);
	if (error)
		return ERR_PTR(error);

	/* If the parent is not specified, we create it in the root.
	 * We need the root dentry to do this, which is in the super
	 * block. A pointer to that is in the struct vfsmount that we
	 * have around.
	 */
	if (!parent)
		parent = tracefs_mount->mnt_root;

	inode_lock(d_inode(parent));
	if (unlikely(IS_DEADDIR(d_inode(parent))))
		dentry = ERR_PTR(-ENOENT);
	else
		dentry = lookup_one_len(name, parent, strlen(name));
	if (!IS_ERR(dentry) && d_inode(dentry)) {
		dput(dentry);
		dentry = ERR_PTR(-EEXIST);
	}

	if (IS_ERR(dentry)) {
		inode_unlock(d_inode(parent));
		simple_release_fs(&tracefs_mount, &tracefs_mount_count);
	}

	return dentry;
}

struct dentry *tracefs_failed_creating(struct dentry *dentry)
{
	inode_unlock(d_inode(dentry->d_parent));
	dput(dentry);
	simple_release_fs(&tracefs_mount, &tracefs_mount_count);
	return NULL;
}

struct dentry *tracefs_end_creating(struct dentry *dentry)
{
	inode_unlock(d_inode(dentry->d_parent));
	return dentry;
}

/**
 * eventfs_start_creating - start the process of creating a dentry
 * @name: Name of the file created for the dentry
 * @parent: The parent dentry where this dentry will be created
 *
 * This is a simple helper function for the dynamically created eventfs
 * files. When the directory of the eventfs files are accessed, their
 * dentries are created on the fly. This function is used to start that
 * process.
 */
struct dentry *eventfs_start_creating(const char *name, struct dentry *parent)
{
	struct dentry *dentry;
	int error;

	error = simple_pin_fs(&trace_fs_type, &tracefs_mount,
			      &tracefs_mount_count);
	if (error)
		return ERR_PTR(error);

	/*
	 * If the parent is not specified, we create it in the root.
	 * We need the root dentry to do this, which is in the super
	 * block. A pointer to that is in the struct vfsmount that we
	 * have around.
	 */
	if (!parent)
		parent = tracefs_mount->mnt_root;

	if (unlikely(IS_DEADDIR(parent->d_inode)))
		dentry = ERR_PTR(-ENOENT);
	else
		dentry = lookup_one_len(name, parent, strlen(name));

	if (!IS_ERR(dentry) && dentry->d_inode) {
		dput(dentry);
		dentry = ERR_PTR(-EEXIST);
	}

	if (IS_ERR(dentry))
		simple_release_fs(&tracefs_mount, &tracefs_mount_count);

	return dentry;
}

/**
 * eventfs_failed_creating - clean up a failed eventfs dentry creation
 * @dentry: The dentry to clean up
 *
 * If after calling eventfs_start_creating(), a failure is detected, the
 * resources created by eventfs_start_creating() needs to be cleaned up. In
 * that case, this function should be called to perform that clean up.
 */
struct dentry *eventfs_failed_creating(struct dentry *dentry)
{
	dput(dentry);
	simple_release_fs(&tracefs_mount, &tracefs_mount_count);
	return NULL;
}

/**
 * eventfs_end_creating - Finish the process of creating a eventfs dentry
 * @dentry: The dentry that has successfully been created.
 *
 * This function is currently just a place holder to match
 * eventfs_start_creating(). In case any synchronization needs to be added,
 * this function will be used to implement that without having to modify
 * the callers of eventfs_start_creating().
 */
struct dentry *eventfs_end_creating(struct dentry *dentry)
{
	return dentry;
}

/**
 * tracefs_create_file - create a file in the tracefs filesystem
 * @name: a pointer to a string containing the name of the file to create.
 * @mode: the permission that the file should have.
 * @parent: a pointer to the parent dentry for this file.  This should be a
 *          directory dentry if set.  If this parameter is NULL, then the
 *          file will be created in the root of the tracefs filesystem.
 * @data: a pointer to something that the caller will want to get to later
 *        on.  The inode.i_private pointer will point to this value on
 *        the open() call.
 * @fops: a pointer to a struct file_operations that should be used for
 *        this file.
 *
 * This is the basic "create a file" function for tracefs.  It allows for a
 * wide range of flexibility in creating a file, or a directory (if you want
 * to create a directory, the tracefs_create_dir() function is
 * recommended to be used instead.)
 *
 * This function will return a pointer to a dentry if it succeeds.  This
 * pointer must be passed to the tracefs_remove() function when the file is
 * to be removed (no automatic cleanup happens if your module is unloaded,
 * you are responsible here.)  If an error occurs, %NULL will be returned.
 *
 * If tracefs is not enabled in the kernel, the value -%ENODEV will be
 * returned.
 */
struct dentry *tracefs_create_file(const char *name, umode_t mode,
				   struct dentry *parent, void *data,
				   const struct file_operations *fops)
{
	struct dentry *dentry;
	struct inode *inode;

	if (security_locked_down(LOCKDOWN_TRACEFS))
		return NULL;

	if (!(mode & S_IFMT))
		mode |= S_IFREG;
	BUG_ON(!S_ISREG(mode));
	dentry = tracefs_start_creating(name, parent);

	if (IS_ERR(dentry))
		return NULL;

	inode = tracefs_get_inode(dentry->d_sb);
	if (unlikely(!inode))
		return tracefs_failed_creating(dentry);

	inode->i_mode = mode;
	inode->i_fop = fops ? fops : &tracefs_file_operations;
	inode->i_private = data;
	inode->i_uid = d_inode(dentry->d_parent)->i_uid;
	inode->i_gid = d_inode(dentry->d_parent)->i_gid;
	d_instantiate(dentry, inode);
	fsnotify_create(d_inode(dentry->d_parent), dentry);
	return tracefs_end_creating(dentry);
}

static struct dentry *__create_dir(const char *name, struct dentry *parent,
				   const struct inode_operations *ops)
{
	struct dentry *dentry = tracefs_start_creating(name, parent);
	struct inode *inode;

	if (IS_ERR(dentry))
		return NULL;

	inode = tracefs_get_inode(dentry->d_sb);
	if (unlikely(!inode))
		return tracefs_failed_creating(dentry);

	/* Do not set bits for OTH */
	inode->i_mode = S_IFDIR | S_IRWXU | S_IRUSR| S_IRGRP | S_IXUSR | S_IXGRP;
	inode->i_op = ops;
	inode->i_fop = &simple_dir_operations;
	inode->i_uid = d_inode(dentry->d_parent)->i_uid;
	inode->i_gid = d_inode(dentry->d_parent)->i_gid;

	/* directory inodes start off with i_nlink == 2 (for "." entry) */
	inc_nlink(inode);
	d_instantiate(dentry, inode);
	inc_nlink(d_inode(dentry->d_parent));
	fsnotify_mkdir(d_inode(dentry->d_parent), dentry);
	return tracefs_end_creating(dentry);
}

/**
 * tracefs_create_dir - create a directory in the tracefs filesystem
 * @name: a pointer to a string containing the name of the directory to
 *        create.
 * @parent: a pointer to the parent dentry for this file.  This should be a
 *          directory dentry if set.  If this parameter is NULL, then the
 *          directory will be created in the root of the tracefs filesystem.
 *
 * This function creates a directory in tracefs with the given name.
 *
 * This function will return a pointer to a dentry if it succeeds.  This
 * pointer must be passed to the tracefs_remove() function when the file is
 * to be removed. If an error occurs, %NULL will be returned.
 *
 * If tracing is not enabled in the kernel, the value -%ENODEV will be
 * returned.
 */
struct dentry *tracefs_create_dir(const char *name, struct dentry *parent)
{
	if (security_locked_down(LOCKDOWN_TRACEFS))
		return NULL;

	return __create_dir(name, parent, &simple_dir_inode_operations);
}

/**
 * tracefs_create_instance_dir - create the tracing instances directory
 * @name: The name of the instances directory to create
 * @parent: The parent directory that the instances directory will exist
 * @mkdir: The function to call when a mkdir is performed.
 * @rmdir: The function to call when a rmdir is performed.
 *
 * Only one instances directory is allowed.
 *
 * The instances directory is special as it allows for mkdir and rmdir
 * to be done by userspace. When a mkdir or rmdir is performed, the inode
 * locks are released and the methods passed in (@mkdir and @rmdir) are
 * called without locks and with the name of the directory being created
 * within the instances directory.
 *
 * Returns the dentry of the instances directory.
 */
__init struct dentry *tracefs_create_instance_dir(const char *name,
					  struct dentry *parent,
					  int (*mkdir)(const char *name),
					  int (*rmdir)(const char *name))
{
	struct dentry *dentry;

	/* Only allow one instance of the instances directory. */
	if (WARN_ON(tracefs_ops.mkdir || tracefs_ops.rmdir))
		return NULL;

	dentry = __create_dir(name, parent, &tracefs_dir_inode_operations);
	if (!dentry)
		return NULL;

	tracefs_ops.mkdir = mkdir;
	tracefs_ops.rmdir = rmdir;

	return dentry;
}

static void remove_one(struct dentry *victim)
{
	simple_release_fs(&tracefs_mount, &tracefs_mount_count);
}

/**
 * tracefs_remove - recursively removes a directory
 * @dentry: a pointer to a the dentry of the directory to be removed.
 *
 * This function recursively removes a directory tree in tracefs that
 * was previously created with a call to another tracefs function
 * (like tracefs_create_file() or variants thereof.)
 */
void tracefs_remove(struct dentry *dentry)
{
	if (IS_ERR_OR_NULL(dentry))
		return;

	simple_pin_fs(&trace_fs_type, &tracefs_mount, &tracefs_mount_count);
	simple_recursive_removal(dentry, remove_one);
	simple_release_fs(&tracefs_mount, &tracefs_mount_count);
}

/**
 * tracefs_initialized - Tells whether tracefs has been registered
 */
bool tracefs_initialized(void)
{
	return tracefs_registered;
}

static void init_once(void *foo)
{
	struct tracefs_inode *ti = (struct tracefs_inode *) foo;

	inode_init_once(&ti->vfs_inode);
}

static int __init tracefs_init(void)
{
	int retval;

	tracefs_inode_cachep = kmem_cache_create("tracefs_inode_cache",
						 sizeof(struct tracefs_inode),
						 0, (SLAB_RECLAIM_ACCOUNT|
						     SLAB_MEM_SPREAD|
						     SLAB_ACCOUNT),
						 init_once);
	if (!tracefs_inode_cachep)
		return -ENOMEM;

	retval = sysfs_create_mount_point(kernel_kobj, "tracing");
	if (retval)
		return -EINVAL;

	retval = register_filesystem(&trace_fs_type);
	if (!retval)
		tracefs_registered = true;

	return retval;
}
core_initcall(tracefs_init);
