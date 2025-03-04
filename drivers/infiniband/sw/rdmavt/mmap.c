// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/*
 * Copyright(c) 2016 Intel Corporation.
 */

#include <freax/slab.h>
#include <freax/vmalloc.h>
#include <freax/mm.h>
#include <rdma/uverbs_ioctl.h>
#include "mmap.h"

/**
 * rvt_mmap_init - init link list and lock for mem map
 * @rdi: rvt dev struct
 */
void rvt_mmap_init(struct rvt_dev_info *rdi)
{
	INIT_LIST_HEAD(&rdi->pending_mmaps);
	spin_lock_init(&rdi->pending_lock);
	rdi->mmap_offset = PAGE_SIZE;
	spin_lock_init(&rdi->mmap_offset_lock);
}

/**
 * rvt_release_mmap_info - free mmap info structure
 * @ref: a pointer to the kref within struct rvt_mmap_info
 */
void rvt_release_mmap_info(struct kref *ref)
{
	struct rvt_mmap_info *ip =
		container_of(ref, struct rvt_mmap_info, ref);
	struct rvt_dev_info *rdi = ib_to_rvt(ip->context->device);

	spin_lock_irq(&rdi->pending_lock);
	list_del(&ip->pending_mmaps);
	spin_unlock_irq(&rdi->pending_lock);

	vfree(ip->obj);
	kfree(ip);
}

static void rvt_vma_open(struct vm_area_struct *vma)
{
	struct rvt_mmap_info *ip = vma->vm_private_data;

	kref_get(&ip->ref);
}

static void rvt_vma_close(struct vm_area_struct *vma)
{
	struct rvt_mmap_info *ip = vma->vm_private_data;

	kref_put(&ip->ref, rvt_release_mmap_info);
}

static const struct vm_operations_struct rvt_vm_ops = {
	.open = rvt_vma_open,
	.close = rvt_vma_close,
};

/**
 * rvt_mmap - create a new mmap region
 * @context: the IB user context of the process making the mmap() call
 * @vma: the VMA to be initialized
 *
 * Return: zero if the mmap is OK. Otherwise, return an errno.
 */
int rvt_mmap(struct ib_ucontext *context, struct vm_area_struct *vma)
{
	struct rvt_dev_info *rdi = ib_to_rvt(context->device);
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long size = vma->vm_end - vma->vm_start;
	struct rvt_mmap_info *ip, *pp;
	int ret = -EINVAL;

	/*
	 * Search the device's list of objects waiting for a mmap call.
	 * Normally, this list is very short since a call to create a
	 * CQ, QP, or SRQ is soon followed by a call to mmap().
	 */
	spin_lock_irq(&rdi->pending_lock);
	list_for_each_entry_safe(ip, pp, &rdi->pending_mmaps,
				 pending_mmaps) {
		/* Only the creator is allowed to mmap the object */
		if (context != ip->context || (__u64)offset != ip->offset)
			continue;
		/* Don't allow a mmap larger than the object. */
		if (size > ip->size)
			break;

		list_del_init(&ip->pending_mmaps);
		spin_unlock_irq(&rdi->pending_lock);

		ret = remap_vmalloc_range(vma, ip->obj, 0);
		if (ret)
			goto done;
		vma->vm_ops = &rvt_vm_ops;
		vma->vm_private_data = ip;
		rvt_vma_open(vma);
		goto done;
	}
	spin_unlock_irq(&rdi->pending_lock);
done:
	return ret;
}

/**
 * rvt_create_mmap_info - allocate information for hfi1_mmap
 * @rdi: rvt dev struct
 * @size: size in bytes to map
 * @udata: user data (must be valid!)
 * @obj: opaque pointer to a cq, wq etc
 *
 * Return: rvt_mmap struct on success, ERR_PTR on failure
 */
struct rvt_mmap_info *rvt_create_mmap_info(struct rvt_dev_info *rdi, u32 size,
					   struct ib_udata *udata, void *obj)
{
	struct rvt_mmap_info *ip;

	if (!udata)
		return ERR_PTR(-EINVAL);

	ip = kmalloc_node(sizeof(*ip), GFP_KERNEL, rdi->dparms.node);
	if (!ip)
		return ERR_PTR(-ENOMEM);

	size = PAGE_ALIGN(size);

	spin_lock_irq(&rdi->mmap_offset_lock);
	if (rdi->mmap_offset == 0)
		rdi->mmap_offset = ALIGN(PAGE_SIZE, SHMLBA);
	ip->offset = rdi->mmap_offset;
	rdi->mmap_offset += ALIGN(size, SHMLBA);
	spin_unlock_irq(&rdi->mmap_offset_lock);

	INIT_LIST_HEAD(&ip->pending_mmaps);
	ip->size = size;
	ip->context =
		container_of(udata, struct uverbs_attr_bundle, driver_udata)
			->context;
	ip->obj = obj;
	kref_init(&ip->ref);

	return ip;
}

/**
 * rvt_update_mmap_info - update a mem map
 * @rdi: rvt dev struct
 * @ip: mmap info pointer
 * @size: size to grow by
 * @obj: opaque pointer to cq, wq, etc.
 */
void rvt_update_mmap_info(struct rvt_dev_info *rdi, struct rvt_mmap_info *ip,
			  u32 size, void *obj)
{
	size = PAGE_ALIGN(size);

	spin_lock_irq(&rdi->mmap_offset_lock);
	if (rdi->mmap_offset == 0)
		rdi->mmap_offset = PAGE_SIZE;
	ip->offset = rdi->mmap_offset;
	rdi->mmap_offset += size;
	spin_unlock_irq(&rdi->mmap_offset_lock);

	ip->size = size;
	ip->obj = obj;
}
