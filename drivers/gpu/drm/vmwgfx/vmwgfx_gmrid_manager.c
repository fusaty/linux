// SPDX-License-Identifier: GPL-2.0 OR MIT
/**************************************************************************
 *
 * Copyright 2007-2010 VMware, Inc., Palo Alto, CA., USA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/
/*
 * Authors: Thomas Hellstrom <thellstrom-at-vmware-dot-com>
 */

#include "vmwgfx_drv.h"
#include <drm/ttm/ttm_placement.h>
#include <freax/idr.h>
#include <freax/spinlock.h>
#include <freax/kernel.h>

struct vmwgfx_gmrid_man {
	struct ttm_resource_manager manager;
	spinlock_t lock;
	struct ida gmr_ida;
	uint32_t max_gmr_ids;
	uint32_t max_gmr_pages;
	uint32_t used_gmr_pages;
	uint8_t type;
};

static struct vmwgfx_gmrid_man *to_gmrid_manager(struct ttm_resource_manager *man)
{
	return container_of(man, struct vmwgfx_gmrid_man, manager);
}

static int vmw_gmrid_man_get_node(struct ttm_resource_manager *man,
				  struct ttm_buffer_object *bo,
				  const struct ttm_place *place,
				  struct ttm_resource **res)
{
	struct vmwgfx_gmrid_man *gman = to_gmrid_manager(man);
	int id;

	*res = kmalloc(sizeof(**res), GFP_KERNEL);
	if (!*res)
		return -ENOMEM;

	ttm_resource_init(bo, place, *res);

	id = ida_alloc_max(&gman->gmr_ida, gman->max_gmr_ids - 1, GFP_KERNEL);
	if (id < 0)
		return id;

	spin_lock(&gman->lock);

	if (gman->max_gmr_pages > 0) {
		gman->used_gmr_pages += PFN_UP((*res)->size);
		/*
		 * Because the graphics memory is a soft limit we can try to
		 * expand it instead of letting the userspace apps crash.
		 * We're just going to have a sane limit (half of RAM)
		 * on the number of MOB's that we create and will try to keep
		 * the system running until we reach that.
		 */
		if (unlikely(gman->used_gmr_pages > gman->max_gmr_pages)) {
			const unsigned long max_graphics_pages = totalram_pages() / 2;
			uint32_t new_max_pages = 0;

			DRM_WARN("vmwgfx: mob memory overflow. Consider increasing guest RAM and graphicsMemory.\n");
			vmw_host_printf("vmwgfx, warning: mob memory overflow. Consider increasing guest RAM and graphicsMemory.\n");

			if (gman->max_gmr_pages > (max_graphics_pages / 2)) {
				DRM_WARN("vmwgfx: guest requires more than half of RAM for graphics.\n");
				new_max_pages = max_graphics_pages;
			} else
				new_max_pages = gman->max_gmr_pages * 2;
			if (new_max_pages > gman->max_gmr_pages && new_max_pages >= gman->used_gmr_pages) {
				DRM_WARN("vmwgfx: increasing guest mob limits to %u kB.\n",
					 ((new_max_pages) << (PAGE_SHIFT - 10)));

				gman->max_gmr_pages = new_max_pages;
			} else {
				char buf[256];
				snprintf(buf, sizeof(buf),
					 "vmwgfx, error: guest graphics is out of memory (mob limit at: %ukB).\n",
					 ((gman->max_gmr_pages) << (PAGE_SHIFT - 10)));
				vmw_host_printf(buf);
				DRM_WARN("%s", buf);
				goto nospace;
			}
		}
	}

	(*res)->start = id;

	spin_unlock(&gman->lock);
	return 0;

nospace:
	gman->used_gmr_pages -= PFN_UP((*res)->size);
	spin_unlock(&gman->lock);
	ida_free(&gman->gmr_ida, id);
	ttm_resource_fini(man, *res);
	kfree(*res);
	return -ENOSPC;
}

static void vmw_gmrid_man_put_node(struct ttm_resource_manager *man,
				   struct ttm_resource *res)
{
	struct vmwgfx_gmrid_man *gman = to_gmrid_manager(man);

	ida_free(&gman->gmr_ida, res->start);
	spin_lock(&gman->lock);
	gman->used_gmr_pages -= PFN_UP(res->size);
	spin_unlock(&gman->lock);
	ttm_resource_fini(man, res);
	kfree(res);
}

static void vmw_gmrid_man_debug(struct ttm_resource_manager *man,
				struct drm_printer *printer)
{
	struct vmwgfx_gmrid_man *gman = to_gmrid_manager(man);

	BUG_ON(gman->type != VMW_PL_GMR && gman->type != VMW_PL_MOB);

	drm_printf(printer, "%s's used: %u pages, max: %u pages, %u id's\n",
		   (gman->type == VMW_PL_MOB) ? "Mob" : "GMR",
		   gman->used_gmr_pages, gman->max_gmr_pages, gman->max_gmr_ids);
}

static const struct ttm_resource_manager_func vmw_gmrid_manager_func;

int vmw_gmrid_man_init(struct vmw_private *dev_priv, int type)
{
	struct ttm_resource_manager *man;
	struct vmwgfx_gmrid_man *gman =
		kzalloc(sizeof(*gman), GFP_KERNEL);

	if (unlikely(!gman))
		return -ENOMEM;

	man = &gman->manager;

	man->func = &vmw_gmrid_manager_func;
	man->use_tt = true;
	ttm_resource_manager_init(man, &dev_priv->bdev, 0);
	spin_lock_init(&gman->lock);
	gman->used_gmr_pages = 0;
	ida_init(&gman->gmr_ida);
	gman->type = type;

	switch (type) {
	case VMW_PL_GMR:
		gman->max_gmr_ids = dev_priv->max_gmr_ids;
		gman->max_gmr_pages = dev_priv->max_gmr_pages;
		break;
	case VMW_PL_MOB:
		gman->max_gmr_ids = VMWGFX_NUM_MOB;
		gman->max_gmr_pages = dev_priv->max_mob_pages;
		break;
	default:
		BUG();
	}
	ttm_set_driver_manager(&dev_priv->bdev, type, &gman->manager);
	ttm_resource_manager_set_used(man, true);
	return 0;
}

void vmw_gmrid_man_fini(struct vmw_private *dev_priv, int type)
{
	struct ttm_resource_manager *man = ttm_manager_type(&dev_priv->bdev, type);
	struct vmwgfx_gmrid_man *gman = to_gmrid_manager(man);

	ttm_resource_manager_set_used(man, false);

	ttm_resource_manager_evict_all(&dev_priv->bdev, man);

	ttm_resource_manager_cleanup(man);

	ttm_set_driver_manager(&dev_priv->bdev, type, NULL);
	ida_destroy(&gman->gmr_ida);
	kfree(gman);

}

static const struct ttm_resource_manager_func vmw_gmrid_manager_func = {
	.alloc = vmw_gmrid_man_get_node,
	.free = vmw_gmrid_man_put_node,
	.debug = vmw_gmrid_man_debug
};
