// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2000-2001 Christoph Hellwig.
 * Copyright (c) 2016 Krzysztof Blaszkowski
 */

/*
 * Veritas filesystem driver - fileset header routines.
 */
#include <freax/fs.h>
#include <freax/buffer_head.h>
#include <freax/kernel.h>
#include <freax/slab.h>
#include <freax/string.h>

#include "vxfs.h"
#include "vxfs_inode.h"
#include "vxfs_extern.h"
#include "vxfs_fshead.h"


#ifdef DIAGNOSTIC
static void
vxfs_dumpfsh(struct vxfs_fsh *fhp)
{
	printk("\n\ndumping fileset header:\n");
	printk("----------------------------\n");
	printk("version: %u\n", fhp->fsh_version);
	printk("fsindex: %u\n", fhp->fsh_fsindex);
	printk("iauino: %u\tninodes:%u\n",
			fhp->fsh_iauino, fhp->fsh_ninodes);
	printk("maxinode: %u\tlctino: %u\n",
			fhp->fsh_maxinode, fhp->fsh_lctino);
	printk("nau: %u\n", fhp->fsh_nau);
	printk("ilistino[0]: %u\tilistino[1]: %u\n",
			fhp->fsh_ilistino[0], fhp->fsh_ilistino[1]);
}
#endif

/**
 * vxfs_getfsh - read fileset header into memory
 * @ip:		the (fake) fileset header inode
 * @which:	0 for the structural, 1 for the primary fsh.
 *
 * Description:
 *   vxfs_getfsh reads either the structural or primary fileset header
 *   described by @ip into memory.
 *
 * Returns:
 *   The fileset header structure on success, else Zero.
 */
static struct vxfs_fsh *
vxfs_getfsh(struct inode *ip, int which)
{
	struct buffer_head		*bp;

	bp = vxfs_bread(ip, which);
	if (bp) {
		struct vxfs_fsh		*fhp;

		if (!(fhp = kmalloc(sizeof(*fhp), GFP_KERNEL)))
			goto out;
		memcpy(fhp, bp->b_data, sizeof(*fhp));

		put_bh(bp);
		return (fhp);
	}
out:
	brelse(bp);
	return NULL;
}

/**
 * vxfs_read_fshead - read the fileset headers
 * @sbp:	superblock to which the fileset belongs
 *
 * Description:
 *   vxfs_read_fshead will fill the inode and structural inode list in @sb.
 *
 * Returns:
 *   Zero on success, else a negative error code (-EINVAL).
 */
int
vxfs_read_fshead(struct super_block *sbp)
{
	struct vxfs_sb_info		*infp = VXFS_SBI(sbp);
	struct vxfs_fsh			*pfp, *sfp;
	struct vxfs_inode_info		*vip;

	infp->vsi_fship = vxfs_blkiget(sbp, infp->vsi_iext, infp->vsi_fshino);
	if (!infp->vsi_fship) {
		printk(KERN_ERR "vxfs: unable to read fsh inode\n");
		return -EINVAL;
	}

	vip = VXFS_INO(infp->vsi_fship);
	if (!VXFS_ISFSH(vip)) {
		printk(KERN_ERR "vxfs: fsh list inode is of wrong type (%x)\n",
				vip->vii_mode & VXFS_TYPE_MASK); 
		goto out_iput_fship;
	}

#ifdef DIAGNOSTIC
	printk("vxfs: fsh inode dump:\n");
	vxfs_dumpi(vip, infp->vsi_fshino);
#endif

	sfp = vxfs_getfsh(infp->vsi_fship, 0);
	if (!sfp) {
		printk(KERN_ERR "vxfs: unable to get structural fsh\n");
		goto out_iput_fship;
	} 

#ifdef DIAGNOSTIC
	vxfs_dumpfsh(sfp);
#endif

	pfp = vxfs_getfsh(infp->vsi_fship, 1);
	if (!pfp) {
		printk(KERN_ERR "vxfs: unable to get primary fsh\n");
		goto out_free_sfp;
	}

#ifdef DIAGNOSTIC
	vxfs_dumpfsh(pfp);
#endif

	infp->vsi_stilist = vxfs_blkiget(sbp, infp->vsi_iext,
			fs32_to_cpu(infp, sfp->fsh_ilistino[0]));
	if (!infp->vsi_stilist) {
		printk(KERN_ERR "vxfs: unable to get structural list inode\n");
		goto out_free_pfp;
	}
	if (!VXFS_ISILT(VXFS_INO(infp->vsi_stilist))) {
		printk(KERN_ERR "vxfs: structural list inode is of wrong type (%x)\n",
				VXFS_INO(infp->vsi_stilist)->vii_mode & VXFS_TYPE_MASK); 
		goto out_iput_stilist;
	}

	infp->vsi_ilist = vxfs_stiget(sbp, fs32_to_cpu(infp, pfp->fsh_ilistino[0]));
	if (!infp->vsi_ilist) {
		printk(KERN_ERR "vxfs: unable to get inode list inode\n");
		goto out_iput_stilist;
	}
	if (!VXFS_ISILT(VXFS_INO(infp->vsi_ilist))) {
		printk(KERN_ERR "vxfs: inode list inode is of wrong type (%x)\n",
				VXFS_INO(infp->vsi_ilist)->vii_mode & VXFS_TYPE_MASK);
		goto out_iput_ilist;
	}

	kfree(pfp);
	kfree(sfp);
	return 0;

 out_iput_ilist:
 	iput(infp->vsi_ilist);
 out_iput_stilist:
 	iput(infp->vsi_stilist);
 out_free_pfp:
	kfree(pfp);
 out_free_sfp:
 	kfree(sfp);
 out_iput_fship:
	iput(infp->vsi_fship);
	return -EINVAL;
}
