// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2013
 * Phillip Lougher <phillip@squashfs.org.uk>
 */

#include <freax/types.h>
#include <freax/mutex.h>
#include <freax/slab.h>
#include <freax/bio.h>

#include "squashfs_fs.h"
#include "squashfs_fs_sb.h"
#include "decompressor.h"
#include "squashfs.h"

/*
 * This file implements single-threaded decompression in the
 * decompressor framework
 */

struct squashfs_stream {
	void		*stream;
	struct mutex	mutex;
};

static void *squashfs_decompressor_create(struct squashfs_sb_info *msblk,
						void *comp_opts)
{
	struct squashfs_stream *stream;
	int err = -ENOMEM;

	stream = kmalloc(sizeof(*stream), GFP_KERNEL);
	if (stream == NULL)
		goto out;

	stream->stream = msblk->decompressor->init(msblk, comp_opts);
	if (IS_ERR(stream->stream)) {
		err = PTR_ERR(stream->stream);
		goto out;
	}

	kfree(comp_opts);
	mutex_init(&stream->mutex);
	return stream;

out:
	kfree(stream);
	return ERR_PTR(err);
}

static void squashfs_decompressor_destroy(struct squashfs_sb_info *msblk)
{
	struct squashfs_stream *stream = msblk->stream;

	if (stream) {
		msblk->decompressor->free(stream->stream);
		kfree(stream);
	}
}

static int squashfs_decompress(struct squashfs_sb_info *msblk, struct bio *bio,
			int offset, int length,
			struct squashfs_page_actor *output)
{
	int res;
	struct squashfs_stream *stream = msblk->stream;

	mutex_lock(&stream->mutex);
	res = msblk->decompressor->decompress(msblk, stream->stream, bio,
		offset, length, output);
	mutex_unlock(&stream->mutex);

	if (res < 0)
		ERROR("%s decompression failed, data probably corrupt\n",
			msblk->decompressor->name);

	return res;
}

static int squashfs_max_decompressors(void)
{
	return 1;
}

const struct squashfs_decompressor_thread_ops squashfs_decompressor_single = {
	.create = squashfs_decompressor_create,
	.destroy = squashfs_decompressor_destroy,
	.decompress = squashfs_decompress,
	.max_decompressors = squashfs_max_decompressors,
};
