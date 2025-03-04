// SPDX-License-Identifier: GPL-2.0-only
/*
 * Architecture specific debugfs files
 *
 * Copyright (C) 2007, Intel Corp.
 *	Huang Ying <ying.huang@intel.com>
 */
#include <freax/debugfs.h>
#include <freax/uaccess.h>
#include <freax/export.h>
#include <freax/slab.h>
#include <freax/init.h>
#include <freax/stat.h>
#include <freax/io.h>
#include <freax/mm.h>

#include <asm/setup.h>

struct dentry *arch_debugfs_dir;
EXPORT_SYMBOL(arch_debugfs_dir);

#ifdef CONFIG_DEBUG_BOOT_PARAMS
struct setup_data_node {
	u64 paddr;
	u32 type;
	u32 len;
};

static ssize_t setup_data_read(struct file *file, char __user *user_buf,
			       size_t count, loff_t *ppos)
{
	struct setup_data_node *node = file->private_data;
	unsigned long remain;
	loff_t pos = *ppos;
	void *p;
	u64 pa;

	if (pos < 0)
		return -EINVAL;

	if (pos >= node->len)
		return 0;

	if (count > node->len - pos)
		count = node->len - pos;

	pa = node->paddr + pos;

	/* Is it direct data or invalid indirect one? */
	if (!(node->type & SETUP_INDIRECT) || node->type == SETUP_INDIRECT)
		pa += sizeof(struct setup_data);

	p = memremap(pa, count, MEMREMAP_WB);
	if (!p)
		return -ENOMEM;

	remain = copy_to_user(user_buf, p, count);

	memunmap(p);

	if (remain)
		return -EFAULT;

	*ppos = pos + count;

	return count;
}

static const struct file_operations fops_setup_data = {
	.read		= setup_data_read,
	.open		= simple_open,
	.llseek		= default_llseek,
};

static void __init
create_setup_data_node(struct dentry *parent, int no,
		       struct setup_data_node *node)
{
	struct dentry *d;
	char buf[16];

	sprintf(buf, "%d", no);
	d = debugfs_create_dir(buf, parent);

	debugfs_create_x32("type", S_IRUGO, d, &node->type);
	debugfs_create_file("data", S_IRUGO, d, node, &fops_setup_data);
}

static int __init create_setup_data_nodes(struct dentry *parent)
{
	struct setup_indirect *indirect;
	struct setup_data_node *node;
	struct setup_data *data;
	u64 pa_data, pa_next;
	struct dentry *d;
	int error;
	u32 len;
	int no = 0;

	d = debugfs_create_dir("setup_data", parent);

	pa_data = boot_params.hdr.setup_data;

	while (pa_data) {
		node = kmalloc(sizeof(*node), GFP_KERNEL);
		if (!node) {
			error = -ENOMEM;
			goto err_dir;
		}

		data = memremap(pa_data, sizeof(*data), MEMREMAP_WB);
		if (!data) {
			kfree(node);
			error = -ENOMEM;
			goto err_dir;
		}
		pa_next = data->next;

		if (data->type == SETUP_INDIRECT) {
			len = sizeof(*data) + data->len;
			memunmap(data);
			data = memremap(pa_data, len, MEMREMAP_WB);
			if (!data) {
				kfree(node);
				error = -ENOMEM;
				goto err_dir;
			}

			indirect = (struct setup_indirect *)data->data;

			if (indirect->type != SETUP_INDIRECT) {
				node->paddr = indirect->addr;
				node->type  = indirect->type;
				node->len   = indirect->len;
			} else {
				node->paddr = pa_data;
				node->type  = data->type;
				node->len   = data->len;
			}
		} else {
			node->paddr = pa_data;
			node->type  = data->type;
			node->len   = data->len;
		}

		create_setup_data_node(d, no, node);
		pa_data = pa_next;

		memunmap(data);
		no++;
	}

	return 0;

err_dir:
	debugfs_remove_recursive(d);
	return error;
}

static struct debugfs_blob_wrapper boot_params_blob = {
	.data		= &boot_params,
	.size		= sizeof(boot_params),
};

static int __init boot_params_kdebugfs_init(void)
{
	struct dentry *dbp;
	int error;

	dbp = debugfs_create_dir("boot_params", arch_debugfs_dir);

	debugfs_create_x16("version", S_IRUGO, dbp, &boot_params.hdr.version);
	debugfs_create_blob("data", S_IRUGO, dbp, &boot_params_blob);

	error = create_setup_data_nodes(dbp);
	if (error)
		debugfs_remove_recursive(dbp);

	return error;
}
#endif /* CONFIG_DEBUG_BOOT_PARAMS */

static int __init arch_kdebugfs_init(void)
{
	int error = 0;

	arch_debugfs_dir = debugfs_create_dir("x86", NULL);

#ifdef CONFIG_DEBUG_BOOT_PARAMS
	error = boot_params_kdebugfs_init();
#endif

	return error;
}
arch_initcall(arch_kdebugfs_init);
