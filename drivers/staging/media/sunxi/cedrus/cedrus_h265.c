// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Cedrus VPU driver
 *
 * Copyright (C) 2013 Jens Kuske <jenskuske@gmail.com>
 * Copyright (C) 2018 Paul Kocialkowski <paul.kocialkowski@bootlin.com>
 * Copyright (C) 2018 Bootlin
 */

#include <freax/delay.h>
#include <freax/types.h>

#include <media/videobuf2-dma-contig.h>

#include "cedrus.h"
#include "cedrus_hw.h"
#include "cedrus_regs.h"

/*
 * These are the sizes for side buffers required by the hardware for storing
 * internal decoding metadata. They match the values used by the early BSP
 * implementations, that were initially exposed in libvdpau-sunxi.
 * Subsequent BSP implementations seem to double the neighbor info buffer size
 * for the H6 SoC, which may be related to 10 bit H265 support.
 */
#define CEDRUS_H265_NEIGHBOR_INFO_BUF_SIZE	(794 * SZ_1K)
#define CEDRUS_H265_ENTRY_POINTS_BUF_SIZE	(4 * SZ_1K)
#define CEDRUS_H265_MV_COL_BUF_UNIT_CTB_SIZE	160

struct cedrus_h265_sram_frame_info {
	__le32	top_pic_order_cnt;
	__le32	bottom_pic_order_cnt;
	__le32	top_mv_col_buf_addr;
	__le32	bottom_mv_col_buf_addr;
	__le32	luma_addr;
	__le32	chroma_addr;
} __packed;

struct cedrus_h265_sram_pred_weight {
	__s8	delta_weight;
	__s8	offset;
} __packed;

static unsigned int cedrus_h265_2bit_size(unsigned int width,
					  unsigned int height)
{
	/*
	 * Vendor library additionally aligns width and height to 16,
	 * but all capture formats are already aligned to that anyway,
	 * so we can skip that here. All formats are also one form of
	 * YUV 4:2:0 or another, so we can safely assume multiplication
	 * factor of 1.5.
	 */
	return ALIGN(width / 4, 32) * height * 3 / 2;
}

static enum cedrus_irq_status cedrus_h265_irq_status(struct cedrus_ctx *ctx)
{
	struct cedrus_dev *dev = ctx->dev;
	u32 reg;

	reg = cedrus_read(dev, VE_DEC_H265_STATUS);
	reg &= VE_DEC_H265_STATUS_CHECK_MASK;

	if (reg & VE_DEC_H265_STATUS_CHECK_ERROR ||
	    !(reg & VE_DEC_H265_STATUS_SUCCESS))
		return CEDRUS_IRQ_ERROR;

	return CEDRUS_IRQ_OK;
}

static void cedrus_h265_irq_clear(struct cedrus_ctx *ctx)
{
	struct cedrus_dev *dev = ctx->dev;

	cedrus_write(dev, VE_DEC_H265_STATUS, VE_DEC_H265_STATUS_CHECK_MASK);
}

static void cedrus_h265_irq_disable(struct cedrus_ctx *ctx)
{
	struct cedrus_dev *dev = ctx->dev;
	u32 reg = cedrus_read(dev, VE_DEC_H265_CTRL);

	reg &= ~VE_DEC_H265_CTRL_IRQ_MASK;

	cedrus_write(dev, VE_DEC_H265_CTRL, reg);
}

static void cedrus_h265_sram_write_offset(struct cedrus_dev *dev, u32 offset)
{
	cedrus_write(dev, VE_DEC_H265_SRAM_OFFSET, offset);
}

static void cedrus_h265_sram_write_data(struct cedrus_dev *dev, void *data,
					unsigned int size)
{
	u32 *word = data;

	while (size >= sizeof(u32)) {
		cedrus_write(dev, VE_DEC_H265_SRAM_DATA, *word++);
		size -= sizeof(u32);
	}
}

static inline dma_addr_t
cedrus_h265_frame_info_mv_col_buf_addr(struct vb2_buffer *buf,
				       unsigned int field)
{
	struct cedrus_buffer *cedrus_buf = vb2_to_cedrus_buffer(buf);

	return cedrus_buf->codec.h265.mv_col_buf_dma +
	       field * cedrus_buf->codec.h265.mv_col_buf_size / 2;
}

static void cedrus_h265_frame_info_write_single(struct cedrus_ctx *ctx,
						unsigned int index,
						bool field_pic,
						u32 pic_order_cnt[],
						struct vb2_buffer *buf)
{
	struct cedrus_dev *dev = ctx->dev;
	dma_addr_t dst_luma_addr = cedrus_dst_buf_addr(ctx, buf, 0);
	dma_addr_t dst_chroma_addr = cedrus_dst_buf_addr(ctx, buf, 1);
	dma_addr_t mv_col_buf_addr[2] = {
		cedrus_h265_frame_info_mv_col_buf_addr(buf, 0),
		cedrus_h265_frame_info_mv_col_buf_addr(buf, field_pic ? 1 : 0)
	};
	u32 offset = VE_DEC_H265_SRAM_OFFSET_FRAME_INFO +
		     VE_DEC_H265_SRAM_OFFSET_FRAME_INFO_UNIT * index;
	struct cedrus_h265_sram_frame_info frame_info = {
		.top_pic_order_cnt = cpu_to_le32(pic_order_cnt[0]),
		.bottom_pic_order_cnt = cpu_to_le32(field_pic ?
						    pic_order_cnt[1] :
						    pic_order_cnt[0]),
		.top_mv_col_buf_addr =
			cpu_to_le32(VE_DEC_H265_SRAM_DATA_ADDR_BASE(mv_col_buf_addr[0])),
		.bottom_mv_col_buf_addr = cpu_to_le32(field_pic ?
			VE_DEC_H265_SRAM_DATA_ADDR_BASE(mv_col_buf_addr[1]) :
			VE_DEC_H265_SRAM_DATA_ADDR_BASE(mv_col_buf_addr[0])),
		.luma_addr = cpu_to_le32(VE_DEC_H265_SRAM_DATA_ADDR_BASE(dst_luma_addr)),
		.chroma_addr = cpu_to_le32(VE_DEC_H265_SRAM_DATA_ADDR_BASE(dst_chroma_addr)),
	};

	cedrus_h265_sram_write_offset(dev, offset);
	cedrus_h265_sram_write_data(dev, &frame_info, sizeof(frame_info));
}

static void cedrus_h265_frame_info_write_dpb(struct cedrus_ctx *ctx,
					     const struct v4l2_hevc_dpb_entry *dpb,
					     u8 num_active_dpb_entries)
{
	struct vb2_queue *vq = v4l2_m2m_get_vq(ctx->fh.m2m_ctx,
					       V4L2_BUF_TYPE_VIDEO_CAPTURE);
	unsigned int i;

	for (i = 0; i < num_active_dpb_entries; i++) {
		struct vb2_buffer *buf = vb2_find_buffer(vq, dpb[i].timestamp);
		u32 pic_order_cnt[2] = {
			dpb[i].pic_order_cnt_val,
			dpb[i].pic_order_cnt_val
		};

		if (!buf)
			continue;

		cedrus_h265_frame_info_write_single(ctx, i, dpb[i].field_pic,
						    pic_order_cnt,
						    buf);
	}
}

static void cedrus_h265_ref_pic_list_write(struct cedrus_dev *dev,
					   const struct v4l2_hevc_dpb_entry *dpb,
					   const u8 list[],
					   u8 num_ref_idx_active,
					   u32 sram_offset)
{
	unsigned int i;
	u32 word = 0;

	cedrus_h265_sram_write_offset(dev, sram_offset);

	for (i = 0; i < num_ref_idx_active; i++) {
		unsigned int shift = (i % 4) * 8;
		unsigned int index = list[i];
		u8 value = list[i];

		if (dpb[index].flags & V4L2_HEVC_DPB_ENTRY_LONG_TERM_REFERENCE)
			value |= VE_DEC_H265_SRAM_REF_PIC_LIST_LT_REF;

		/* Each SRAM word gathers up to 4 references. */
		word |= value << shift;

		/* Write the word to SRAM and clear it for the next batch. */
		if ((i % 4) == 3 || i == (num_ref_idx_active - 1)) {
			cedrus_h265_sram_write_data(dev, &word, sizeof(word));
			word = 0;
		}
	}
}

static void cedrus_h265_pred_weight_write(struct cedrus_dev *dev,
					  const s8 delta_luma_weight[],
					  const s8 luma_offset[],
					  const s8 delta_chroma_weight[][2],
					  const s8 chroma_offset[][2],
					  u8 num_ref_idx_active,
					  u32 sram_luma_offset,
					  u32 sram_chroma_offset)
{
	struct cedrus_h265_sram_pred_weight pred_weight[2] = { { 0 } };
	unsigned int i, j;

	cedrus_h265_sram_write_offset(dev, sram_luma_offset);

	for (i = 0; i < num_ref_idx_active; i++) {
		unsigned int index = i % 2;

		pred_weight[index].delta_weight = delta_luma_weight[i];
		pred_weight[index].offset = luma_offset[i];

		if (index == 1 || i == (num_ref_idx_active - 1))
			cedrus_h265_sram_write_data(dev, (u32 *)&pred_weight,
						    sizeof(pred_weight));
	}

	cedrus_h265_sram_write_offset(dev, sram_chroma_offset);

	for (i = 0; i < num_ref_idx_active; i++) {
		for (j = 0; j < 2; j++) {
			pred_weight[j].delta_weight = delta_chroma_weight[i][j];
			pred_weight[j].offset = chroma_offset[i][j];
		}

		cedrus_h265_sram_write_data(dev, &pred_weight,
					    sizeof(pred_weight));
	}
}

static void cedrus_h265_skip_bits(struct cedrus_dev *dev, int num)
{
	int count = 0;

	while (count < num) {
		int tmp = min(num - count, 32);

		cedrus_write(dev, VE_DEC_H265_TRIGGER,
			     VE_DEC_H265_TRIGGER_FLUSH_BITS |
			     VE_DEC_H265_TRIGGER_TYPE_N_BITS(tmp));

		if (cedrus_wait_for(dev, VE_DEC_H265_STATUS, VE_DEC_H265_STATUS_VLD_BUSY))
			dev_err_ratelimited(dev->dev, "timed out waiting to skip bits\n");

		count += tmp;
	}
}

static u32 cedrus_h265_show_bits(struct cedrus_dev *dev, int num)
{
	cedrus_write(dev, VE_DEC_H265_TRIGGER,
		     VE_DEC_H265_TRIGGER_SHOW_BITS |
		     VE_DEC_H265_TRIGGER_TYPE_N_BITS(num));

	cedrus_wait_for(dev, VE_DEC_H265_STATUS,
			VE_DEC_H265_STATUS_VLD_BUSY);

	return cedrus_read(dev, VE_DEC_H265_BITS_READ);
}

static void cedrus_h265_write_scaling_list(struct cedrus_ctx *ctx,
					   struct cedrus_run *run)
{
	const struct v4l2_ctrl_hevc_scaling_matrix *scaling;
	struct cedrus_dev *dev = ctx->dev;
	u32 i, j, k, val;

	scaling = run->h265.scaling_matrix;

	cedrus_write(dev, VE_DEC_H265_SCALING_LIST_DC_COEF0,
		     (scaling->scaling_list_dc_coef_32x32[1] << 24) |
		     (scaling->scaling_list_dc_coef_32x32[0] << 16) |
		     (scaling->scaling_list_dc_coef_16x16[1] << 8) |
		     (scaling->scaling_list_dc_coef_16x16[0] << 0));

	cedrus_write(dev, VE_DEC_H265_SCALING_LIST_DC_COEF1,
		     (scaling->scaling_list_dc_coef_16x16[5] << 24) |
		     (scaling->scaling_list_dc_coef_16x16[4] << 16) |
		     (scaling->scaling_list_dc_coef_16x16[3] << 8) |
		     (scaling->scaling_list_dc_coef_16x16[2] << 0));

	cedrus_h265_sram_write_offset(dev, VE_DEC_H265_SRAM_OFFSET_SCALING_LISTS);

	for (i = 0; i < 6; i++)
		for (j = 0; j < 8; j++)
			for (k = 0; k < 8; k += 4) {
				val = ((u32)scaling->scaling_list_8x8[i][j + (k + 3) * 8] << 24) |
				      ((u32)scaling->scaling_list_8x8[i][j + (k + 2) * 8] << 16) |
				      ((u32)scaling->scaling_list_8x8[i][j + (k + 1) * 8] << 8) |
				      scaling->scaling_list_8x8[i][j + k * 8];
				cedrus_write(dev, VE_DEC_H265_SRAM_DATA, val);
			}

	for (i = 0; i < 2; i++)
		for (j = 0; j < 8; j++)
			for (k = 0; k < 8; k += 4) {
				val = ((u32)scaling->scaling_list_32x32[i][j + (k + 3) * 8] << 24) |
				      ((u32)scaling->scaling_list_32x32[i][j + (k + 2) * 8] << 16) |
				      ((u32)scaling->scaling_list_32x32[i][j + (k + 1) * 8] << 8) |
				      scaling->scaling_list_32x32[i][j + k * 8];
				cedrus_write(dev, VE_DEC_H265_SRAM_DATA, val);
			}

	for (i = 0; i < 6; i++)
		for (j = 0; j < 8; j++)
			for (k = 0; k < 8; k += 4) {
				val = ((u32)scaling->scaling_list_16x16[i][j + (k + 3) * 8] << 24) |
				      ((u32)scaling->scaling_list_16x16[i][j + (k + 2) * 8] << 16) |
				      ((u32)scaling->scaling_list_16x16[i][j + (k + 1) * 8] << 8) |
				      scaling->scaling_list_16x16[i][j + k * 8];
				cedrus_write(dev, VE_DEC_H265_SRAM_DATA, val);
			}

	for (i = 0; i < 6; i++)
		for (j = 0; j < 4; j++) {
			val = ((u32)scaling->scaling_list_4x4[i][j + 12] << 24) |
			      ((u32)scaling->scaling_list_4x4[i][j + 8] << 16) |
			      ((u32)scaling->scaling_list_4x4[i][j + 4] << 8) |
			      scaling->scaling_list_4x4[i][j];
			cedrus_write(dev, VE_DEC_H265_SRAM_DATA, val);
		}
}

static int cedrus_h265_is_low_delay(struct cedrus_run *run)
{
	const struct v4l2_ctrl_hevc_slice_params *slice_params;
	const struct v4l2_hevc_dpb_entry *dpb;
	s32 poc;
	int i;

	slice_params = run->h265.slice_params;
	poc = run->h265.decode_params->pic_order_cnt_val;
	dpb = run->h265.decode_params->dpb;

	for (i = 0; i < slice_params->num_ref_idx_l0_active_minus1 + 1; i++)
		if (dpb[slice_params->ref_idx_l0[i]].pic_order_cnt_val > poc)
			return 1;

	if (slice_params->slice_type != V4L2_HEVC_SLICE_TYPE_B)
		return 0;

	for (i = 0; i < slice_params->num_ref_idx_l1_active_minus1 + 1; i++)
		if (dpb[slice_params->ref_idx_l1[i]].pic_order_cnt_val > poc)
			return 1;

	return 0;
}

static void cedrus_h265_write_tiles(struct cedrus_ctx *ctx,
				    struct cedrus_run *run,
				    unsigned int ctb_addr_x,
				    unsigned int ctb_addr_y)
{
	const struct v4l2_ctrl_hevc_slice_params *slice_params;
	const struct v4l2_ctrl_hevc_pps *pps;
	struct cedrus_dev *dev = ctx->dev;
	const u32 *entry_points;
	u32 *entry_points_buf;
	int i, x, tx, y, ty;

	pps = run->h265.pps;
	slice_params = run->h265.slice_params;
	entry_points = run->h265.entry_points;
	entry_points_buf = ctx->codec.h265.entry_points_buf;

	for (x = 0, tx = 0; tx < pps->num_tile_columns_minus1 + 1; tx++) {
		if (x + pps->column_width_minus1[tx] + 1 > ctb_addr_x)
			break;

		x += pps->column_width_minus1[tx] + 1;
	}

	for (y = 0, ty = 0; ty < pps->num_tile_rows_minus1 + 1; ty++) {
		if (y + pps->row_height_minus1[ty] + 1 > ctb_addr_y)
			break;

		y += pps->row_height_minus1[ty] + 1;
	}

	cedrus_write(dev, VE_DEC_H265_TILE_START_CTB, (y << 16) | (x << 0));
	cedrus_write(dev, VE_DEC_H265_TILE_END_CTB,
		     ((y + pps->row_height_minus1[ty]) << 16) |
		     ((x + pps->column_width_minus1[tx]) << 0));

	if (pps->flags & V4L2_HEVC_PPS_FLAG_ENTROPY_CODING_SYNC_ENABLED) {
		for (i = 0; i < slice_params->num_entry_point_offsets; i++)
			entry_points_buf[i] = entry_points[i];
	} else {
		for (i = 0; i < slice_params->num_entry_point_offsets; i++) {
			if (tx + 1 >= pps->num_tile_columns_minus1 + 1) {
				x = 0;
				tx = 0;
				y += pps->row_height_minus1[ty++] + 1;
			} else {
				x += pps->column_width_minus1[tx++] + 1;
			}

			entry_points_buf[i * 4 + 0] = entry_points[i];
			entry_points_buf[i * 4 + 1] = 0x0;
			entry_points_buf[i * 4 + 2] = (y << 16) | (x << 0);
			entry_points_buf[i * 4 + 3] =
				((y + pps->row_height_minus1[ty]) << 16) |
				((x + pps->column_width_minus1[tx]) << 0);
		}
	}
}

static int cedrus_h265_setup(struct cedrus_ctx *ctx, struct cedrus_run *run)
{
	struct cedrus_dev *dev = ctx->dev;
	const struct v4l2_ctrl_hevc_sps *sps;
	const struct v4l2_ctrl_hevc_pps *pps;
	const struct v4l2_ctrl_hevc_slice_params *slice_params;
	const struct v4l2_ctrl_hevc_decode_params *decode_params;
	const struct v4l2_hevc_pred_weight_table *pred_weight_table;
	unsigned int width_in_ctb_luma, ctb_size_luma;
	unsigned int log2_max_luma_coding_block_size;
	unsigned int ctb_addr_x, ctb_addr_y;
	struct cedrus_buffer *cedrus_buf;
	dma_addr_t src_buf_addr;
	dma_addr_t src_buf_end_addr;
	u32 chroma_log2_weight_denom;
	u32 num_entry_point_offsets;
	u32 output_pic_list_index;
	u32 pic_order_cnt[2];
	u8 padding;
	int count;
	u32 reg;

	sps = run->h265.sps;
	pps = run->h265.pps;
	slice_params = run->h265.slice_params;
	decode_params = run->h265.decode_params;
	pred_weight_table = &slice_params->pred_weight_table;
	num_entry_point_offsets = slice_params->num_entry_point_offsets;
	cedrus_buf = vb2_to_cedrus_buffer(&run->dst->vb2_buf);

	/*
	 * If entry points offsets are present, we should get them
	 * exactly the right amount.
	 */
	if (num_entry_point_offsets &&
	    num_entry_point_offsets != run->h265.entry_points_count)
		return -ERANGE;

	log2_max_luma_coding_block_size =
		sps->log2_min_luma_coding_block_size_minus3 + 3 +
		sps->log2_diff_max_min_luma_coding_block_size;
	ctb_size_luma = 1UL << log2_max_luma_coding_block_size;
	width_in_ctb_luma =
		DIV_ROUND_UP(sps->pic_width_in_luma_samples, ctb_size_luma);

	/* MV column buffer size and allocation. */
	if (!cedrus_buf->codec.h265.mv_col_buf_size) {
		/*
		 * Each CTB requires a MV col buffer with a specific unit size.
		 * Since the address is given with missing lsb bits, 1 KiB is
		 * added to each buffer to ensure proper alignment.
		 */
		cedrus_buf->codec.h265.mv_col_buf_size =
			DIV_ROUND_UP(ctx->src_fmt.width, ctb_size_luma) *
			DIV_ROUND_UP(ctx->src_fmt.height, ctb_size_luma) *
			CEDRUS_H265_MV_COL_BUF_UNIT_CTB_SIZE + SZ_1K;

		/* Buffer is never accessed by CPU, so we can skip kernel mapping. */
		cedrus_buf->codec.h265.mv_col_buf =
			dma_alloc_attrs(dev->dev,
					cedrus_buf->codec.h265.mv_col_buf_size,
					&cedrus_buf->codec.h265.mv_col_buf_dma,
					GFP_KERNEL, DMA_ATTR_NO_KERNEL_MAPPING);
		if (!cedrus_buf->codec.h265.mv_col_buf) {
			cedrus_buf->codec.h265.mv_col_buf_size = 0;
			return -ENOMEM;
		}
	}

	/* Activate H265 engine. */
	cedrus_engine_enable(ctx);

	/* Source offset and length in bits. */

	cedrus_write(dev, VE_DEC_H265_BITS_OFFSET, 0);

	reg = slice_params->bit_size;
	cedrus_write(dev, VE_DEC_H265_BITS_LEN, reg);

	/* Source beginning and end addresses. */

	src_buf_addr = vb2_dma_contig_plane_dma_addr(&run->src->vb2_buf, 0);

	reg = VE_DEC_H265_BITS_ADDR_BASE(src_buf_addr);
	reg |= VE_DEC_H265_BITS_ADDR_VALID_SLICE_DATA;
	reg |= VE_DEC_H265_BITS_ADDR_LAST_SLICE_DATA;
	reg |= VE_DEC_H265_BITS_ADDR_FIRST_SLICE_DATA;

	cedrus_write(dev, VE_DEC_H265_BITS_ADDR, reg);

	src_buf_end_addr = src_buf_addr +
			   DIV_ROUND_UP(slice_params->bit_size, 8);

	reg = VE_DEC_H265_BITS_END_ADDR_BASE(src_buf_end_addr);
	cedrus_write(dev, VE_DEC_H265_BITS_END_ADDR, reg);

	/* Coding tree block address */
	ctb_addr_x = slice_params->slice_segment_addr % width_in_ctb_luma;
	ctb_addr_y = slice_params->slice_segment_addr / width_in_ctb_luma;
	reg = VE_DEC_H265_DEC_CTB_ADDR_X(ctb_addr_x);
	reg |= VE_DEC_H265_DEC_CTB_ADDR_Y(ctb_addr_y);
	cedrus_write(dev, VE_DEC_H265_DEC_CTB_ADDR, reg);

	if ((pps->flags & V4L2_HEVC_PPS_FLAG_TILES_ENABLED) ||
	    (pps->flags & V4L2_HEVC_PPS_FLAG_ENTROPY_CODING_SYNC_ENABLED)) {
		cedrus_h265_write_tiles(ctx, run, ctb_addr_x, ctb_addr_y);
	} else {
		cedrus_write(dev, VE_DEC_H265_TILE_START_CTB, 0);
		cedrus_write(dev, VE_DEC_H265_TILE_END_CTB, 0);
	}

	/* Clear the number of correctly-decoded coding tree blocks. */
	if (ctx->fh.m2m_ctx->new_frame)
		cedrus_write(dev, VE_DEC_H265_DEC_CTB_NUM, 0);

	/* Initialize bitstream access. */
	cedrus_write(dev, VE_DEC_H265_TRIGGER, VE_DEC_H265_TRIGGER_INIT_SWDEC);

	/*
	 * Cedrus expects that bitstream pointer is actually at the end of the slice header
	 * instead of start of slice data. Padding is 8 bits at most (one bit set to 1 and
	 * at most seven bits set to 0), so we have to inspect only one byte before slice data.
	 */

	if (slice_params->data_byte_offset == 0)
		return -EOPNOTSUPP;

	cedrus_h265_skip_bits(dev, (slice_params->data_byte_offset - 1) * 8);

	padding = cedrus_h265_show_bits(dev, 8);

	/* at least one bit must be set in that byte */
	if (padding == 0)
		return -EINVAL;

	for (count = 0; count < 8; count++)
		if (padding & (1 << count))
			break;

	/* Include the one bit. */
	count++;

	cedrus_h265_skip_bits(dev, 8 - count);

	/* Bitstream parameters. */

	reg = VE_DEC_H265_DEC_NAL_HDR_NAL_UNIT_TYPE(slice_params->nal_unit_type) |
	      VE_DEC_H265_DEC_NAL_HDR_NUH_TEMPORAL_ID_PLUS1(slice_params->nuh_temporal_id_plus1);

	cedrus_write(dev, VE_DEC_H265_DEC_NAL_HDR, reg);

	/* SPS. */

	reg = VE_DEC_H265_DEC_SPS_HDR_MAX_TRANSFORM_HIERARCHY_DEPTH_INTRA(sps->max_transform_hierarchy_depth_intra) |
	      VE_DEC_H265_DEC_SPS_HDR_MAX_TRANSFORM_HIERARCHY_DEPTH_INTER(sps->max_transform_hierarchy_depth_inter) |
	      VE_DEC_H265_DEC_SPS_HDR_LOG2_DIFF_MAX_MIN_TRANSFORM_BLOCK_SIZE(sps->log2_diff_max_min_luma_transform_block_size) |
	      VE_DEC_H265_DEC_SPS_HDR_LOG2_MIN_TRANSFORM_BLOCK_SIZE_MINUS2(sps->log2_min_luma_transform_block_size_minus2) |
	      VE_DEC_H265_DEC_SPS_HDR_LOG2_DIFF_MAX_MIN_LUMA_CODING_BLOCK_SIZE(sps->log2_diff_max_min_luma_coding_block_size) |
	      VE_DEC_H265_DEC_SPS_HDR_LOG2_MIN_LUMA_CODING_BLOCK_SIZE_MINUS3(sps->log2_min_luma_coding_block_size_minus3) |
	      VE_DEC_H265_DEC_SPS_HDR_BIT_DEPTH_CHROMA_MINUS8(sps->bit_depth_chroma_minus8) |
	      VE_DEC_H265_DEC_SPS_HDR_BIT_DEPTH_LUMA_MINUS8(sps->bit_depth_luma_minus8) |
	      VE_DEC_H265_DEC_SPS_HDR_CHROMA_FORMAT_IDC(sps->chroma_format_idc);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SPS_HDR_FLAG_STRONG_INTRA_SMOOTHING_ENABLE,
				V4L2_HEVC_SPS_FLAG_STRONG_INTRA_SMOOTHING_ENABLED,
				sps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SPS_HDR_FLAG_SPS_TEMPORAL_MVP_ENABLED,
				V4L2_HEVC_SPS_FLAG_SPS_TEMPORAL_MVP_ENABLED,
				sps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SPS_HDR_FLAG_SAMPLE_ADAPTIVE_OFFSET_ENABLED,
				V4L2_HEVC_SPS_FLAG_SAMPLE_ADAPTIVE_OFFSET,
				sps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SPS_HDR_FLAG_AMP_ENABLED,
				V4L2_HEVC_SPS_FLAG_AMP_ENABLED, sps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SPS_HDR_FLAG_SEPARATE_COLOUR_PLANE,
				V4L2_HEVC_SPS_FLAG_SEPARATE_COLOUR_PLANE,
				sps->flags);

	cedrus_write(dev, VE_DEC_H265_DEC_SPS_HDR, reg);

	reg = VE_DEC_H265_DEC_PCM_CTRL_LOG2_DIFF_MAX_MIN_PCM_LUMA_CODING_BLOCK_SIZE(sps->log2_diff_max_min_pcm_luma_coding_block_size) |
	      VE_DEC_H265_DEC_PCM_CTRL_LOG2_MIN_PCM_LUMA_CODING_BLOCK_SIZE_MINUS3(sps->log2_min_pcm_luma_coding_block_size_minus3) |
	      VE_DEC_H265_DEC_PCM_CTRL_PCM_SAMPLE_BIT_DEPTH_CHROMA_MINUS1(sps->pcm_sample_bit_depth_chroma_minus1) |
	      VE_DEC_H265_DEC_PCM_CTRL_PCM_SAMPLE_BIT_DEPTH_LUMA_MINUS1(sps->pcm_sample_bit_depth_luma_minus1);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PCM_CTRL_FLAG_PCM_ENABLED,
				V4L2_HEVC_SPS_FLAG_PCM_ENABLED, sps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PCM_CTRL_FLAG_PCM_LOOP_FILTER_DISABLED,
				V4L2_HEVC_SPS_FLAG_PCM_LOOP_FILTER_DISABLED,
				sps->flags);

	cedrus_write(dev, VE_DEC_H265_DEC_PCM_CTRL, reg);

	/* PPS. */

	reg = VE_DEC_H265_DEC_PPS_CTRL0_PPS_CR_QP_OFFSET(pps->pps_cr_qp_offset) |
	      VE_DEC_H265_DEC_PPS_CTRL0_PPS_CB_QP_OFFSET(pps->pps_cb_qp_offset) |
	      VE_DEC_H265_DEC_PPS_CTRL0_INIT_QP_MINUS26(pps->init_qp_minus26) |
	      VE_DEC_H265_DEC_PPS_CTRL0_DIFF_CU_QP_DELTA_DEPTH(pps->diff_cu_qp_delta_depth);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL0_FLAG_CU_QP_DELTA_ENABLED,
				V4L2_HEVC_PPS_FLAG_CU_QP_DELTA_ENABLED,
				pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL0_FLAG_TRANSFORM_SKIP_ENABLED,
				V4L2_HEVC_PPS_FLAG_TRANSFORM_SKIP_ENABLED,
				pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL0_FLAG_CONSTRAINED_INTRA_PRED,
				V4L2_HEVC_PPS_FLAG_CONSTRAINED_INTRA_PRED,
				pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL0_FLAG_SIGN_DATA_HIDING_ENABLED,
				V4L2_HEVC_PPS_FLAG_SIGN_DATA_HIDING_ENABLED,
				pps->flags);

	cedrus_write(dev, VE_DEC_H265_DEC_PPS_CTRL0, reg);

	reg = VE_DEC_H265_DEC_PPS_CTRL1_LOG2_PARALLEL_MERGE_LEVEL_MINUS2(pps->log2_parallel_merge_level_minus2);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL1_FLAG_PPS_LOOP_FILTER_ACROSS_SLICES_ENABLED,
				V4L2_HEVC_PPS_FLAG_PPS_LOOP_FILTER_ACROSS_SLICES_ENABLED,
				pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL1_FLAG_LOOP_FILTER_ACROSS_TILES_ENABLED,
				V4L2_HEVC_PPS_FLAG_LOOP_FILTER_ACROSS_TILES_ENABLED,
				pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL1_FLAG_ENTROPY_CODING_SYNC_ENABLED,
				V4L2_HEVC_PPS_FLAG_ENTROPY_CODING_SYNC_ENABLED,
				pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL1_FLAG_TILES_ENABLED,
				V4L2_HEVC_PPS_FLAG_TILES_ENABLED,
				pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL1_FLAG_TRANSQUANT_BYPASS_ENABLED,
				V4L2_HEVC_PPS_FLAG_TRANSQUANT_BYPASS_ENABLED,
				pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL1_FLAG_WEIGHTED_BIPRED,
				V4L2_HEVC_PPS_FLAG_WEIGHTED_BIPRED, pps->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_PPS_CTRL1_FLAG_WEIGHTED_PRED,
				V4L2_HEVC_PPS_FLAG_WEIGHTED_PRED, pps->flags);

	cedrus_write(dev, VE_DEC_H265_DEC_PPS_CTRL1, reg);

	/* Slice Parameters. */

	reg = VE_DEC_H265_DEC_SLICE_HDR_INFO0_PICTURE_TYPE(slice_params->pic_struct) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO0_FIVE_MINUS_MAX_NUM_MERGE_CAND(slice_params->five_minus_max_num_merge_cand) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO0_NUM_REF_IDX_L1_ACTIVE_MINUS1(slice_params->num_ref_idx_l1_active_minus1) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO0_NUM_REF_IDX_L0_ACTIVE_MINUS1(slice_params->num_ref_idx_l0_active_minus1) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO0_COLLOCATED_REF_IDX(slice_params->collocated_ref_idx) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO0_COLOUR_PLANE_ID(slice_params->colour_plane_id) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO0_SLICE_TYPE(slice_params->slice_type);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO0_FLAG_COLLOCATED_FROM_L0,
				V4L2_HEVC_SLICE_PARAMS_FLAG_COLLOCATED_FROM_L0,
				slice_params->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO0_FLAG_CABAC_INIT,
				V4L2_HEVC_SLICE_PARAMS_FLAG_CABAC_INIT,
				slice_params->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO0_FLAG_MVD_L1_ZERO,
				V4L2_HEVC_SLICE_PARAMS_FLAG_MVD_L1_ZERO,
				slice_params->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO0_FLAG_SLICE_SAO_CHROMA,
				V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_SAO_CHROMA,
				slice_params->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO0_FLAG_SLICE_SAO_LUMA,
				V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_SAO_LUMA,
				slice_params->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO0_FLAG_SLICE_TEMPORAL_MVP_ENABLE,
				V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_TEMPORAL_MVP_ENABLED,
				slice_params->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO0_FLAG_DEPENDENT_SLICE_SEGMENT,
				V4L2_HEVC_SLICE_PARAMS_FLAG_DEPENDENT_SLICE_SEGMENT,
				slice_params->flags);

	if (ctx->fh.m2m_ctx->new_frame)
		reg |= VE_DEC_H265_DEC_SLICE_HDR_INFO0_FLAG_FIRST_SLICE_SEGMENT_IN_PIC;

	cedrus_write(dev, VE_DEC_H265_DEC_SLICE_HDR_INFO0, reg);

	reg = VE_DEC_H265_DEC_SLICE_HDR_INFO1_SLICE_TC_OFFSET_DIV2(slice_params->slice_tc_offset_div2) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO1_SLICE_BETA_OFFSET_DIV2(slice_params->slice_beta_offset_div2) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO1_SLICE_CR_QP_OFFSET(slice_params->slice_cr_qp_offset) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO1_SLICE_CB_QP_OFFSET(slice_params->slice_cb_qp_offset) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO1_SLICE_QP_DELTA(slice_params->slice_qp_delta);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO1_FLAG_SLICE_DEBLOCKING_FILTER_DISABLED,
				V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_DEBLOCKING_FILTER_DISABLED,
				slice_params->flags);

	reg |= VE_DEC_H265_FLAG(VE_DEC_H265_DEC_SLICE_HDR_INFO1_FLAG_SLICE_LOOP_FILTER_ACROSS_SLICES_ENABLED,
				V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_LOOP_FILTER_ACROSS_SLICES_ENABLED,
				slice_params->flags);

	if (slice_params->slice_type != V4L2_HEVC_SLICE_TYPE_I && !cedrus_h265_is_low_delay(run))
		reg |= VE_DEC_H265_DEC_SLICE_HDR_INFO1_FLAG_SLICE_NOT_LOW_DELAY;

	cedrus_write(dev, VE_DEC_H265_DEC_SLICE_HDR_INFO1, reg);

	chroma_log2_weight_denom = pred_weight_table->luma_log2_weight_denom +
				   pred_weight_table->delta_chroma_log2_weight_denom;
	reg = VE_DEC_H265_DEC_SLICE_HDR_INFO2_NUM_ENTRY_POINT_OFFSETS(num_entry_point_offsets) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO2_CHROMA_LOG2_WEIGHT_DENOM(chroma_log2_weight_denom) |
	      VE_DEC_H265_DEC_SLICE_HDR_INFO2_LUMA_LOG2_WEIGHT_DENOM(pred_weight_table->luma_log2_weight_denom);

	cedrus_write(dev, VE_DEC_H265_DEC_SLICE_HDR_INFO2, reg);

	cedrus_write(dev, VE_DEC_H265_ENTRY_POINT_OFFSET_ADDR,
		     ctx->codec.h265.entry_points_buf_addr >> 8);

	/* Decoded picture size. */

	reg = VE_DEC_H265_DEC_PIC_SIZE_WIDTH(ctx->src_fmt.width) |
	      VE_DEC_H265_DEC_PIC_SIZE_HEIGHT(ctx->src_fmt.height);

	cedrus_write(dev, VE_DEC_H265_DEC_PIC_SIZE, reg);

	/* Scaling list. */

	if (sps->flags & V4L2_HEVC_SPS_FLAG_SCALING_LIST_ENABLED) {
		cedrus_h265_write_scaling_list(ctx, run);
		reg = VE_DEC_H265_SCALING_LIST_CTRL0_FLAG_ENABLED;
	} else {
		reg = VE_DEC_H265_SCALING_LIST_CTRL0_DEFAULT;
	}
	cedrus_write(dev, VE_DEC_H265_SCALING_LIST_CTRL0, reg);

	/* Neightbor information address. */
	reg = VE_DEC_H265_NEIGHBOR_INFO_ADDR_BASE(ctx->codec.h265.neighbor_info_buf_addr);
	cedrus_write(dev, VE_DEC_H265_NEIGHBOR_INFO_ADDR, reg);

	/* Write decoded picture buffer in pic list. */
	cedrus_h265_frame_info_write_dpb(ctx, decode_params->dpb,
					 decode_params->num_active_dpb_entries);

	/* Output frame. */

	output_pic_list_index = V4L2_HEVC_DPB_ENTRIES_NUM_MAX;
	pic_order_cnt[0] = slice_params->slice_pic_order_cnt;
	pic_order_cnt[1] = slice_params->slice_pic_order_cnt;

	cedrus_h265_frame_info_write_single(ctx, output_pic_list_index,
					    slice_params->pic_struct != 0,
					    pic_order_cnt,
					    &run->dst->vb2_buf);

	cedrus_write(dev, VE_DEC_H265_OUTPUT_FRAME_IDX, output_pic_list_index);

	/* Reference picture list 0 (for P/B frames). */
	if (slice_params->slice_type != V4L2_HEVC_SLICE_TYPE_I) {
		cedrus_h265_ref_pic_list_write(dev, decode_params->dpb,
					       slice_params->ref_idx_l0,
					       slice_params->num_ref_idx_l0_active_minus1 + 1,
					       VE_DEC_H265_SRAM_OFFSET_REF_PIC_LIST0);

		if ((pps->flags & V4L2_HEVC_PPS_FLAG_WEIGHTED_PRED) ||
		    (pps->flags & V4L2_HEVC_PPS_FLAG_WEIGHTED_BIPRED))
			cedrus_h265_pred_weight_write(dev,
						      pred_weight_table->delta_luma_weight_l0,
						      pred_weight_table->luma_offset_l0,
						      pred_weight_table->delta_chroma_weight_l0,
						      pred_weight_table->chroma_offset_l0,
						      slice_params->num_ref_idx_l0_active_minus1 + 1,
						      VE_DEC_H265_SRAM_OFFSET_PRED_WEIGHT_LUMA_L0,
						      VE_DEC_H265_SRAM_OFFSET_PRED_WEIGHT_CHROMA_L0);
	}

	/* Reference picture list 1 (for B frames). */
	if (slice_params->slice_type == V4L2_HEVC_SLICE_TYPE_B) {
		cedrus_h265_ref_pic_list_write(dev, decode_params->dpb,
					       slice_params->ref_idx_l1,
					       slice_params->num_ref_idx_l1_active_minus1 + 1,
					       VE_DEC_H265_SRAM_OFFSET_REF_PIC_LIST1);

		if (pps->flags & V4L2_HEVC_PPS_FLAG_WEIGHTED_BIPRED)
			cedrus_h265_pred_weight_write(dev,
						      pred_weight_table->delta_luma_weight_l1,
						      pred_weight_table->luma_offset_l1,
						      pred_weight_table->delta_chroma_weight_l1,
						      pred_weight_table->chroma_offset_l1,
						      slice_params->num_ref_idx_l1_active_minus1 + 1,
						      VE_DEC_H265_SRAM_OFFSET_PRED_WEIGHT_LUMA_L1,
						      VE_DEC_H265_SRAM_OFFSET_PRED_WEIGHT_CHROMA_L1);
	}

	if (ctx->bit_depth > 8) {
		unsigned int stride = ALIGN(ctx->dst_fmt.width / 4, 32);

		reg = ctx->dst_fmt.sizeimage -
		      cedrus_h265_2bit_size(ctx->dst_fmt.width,
					    ctx->dst_fmt.height);
		cedrus_write(dev, VE_DEC_H265_OFFSET_ADDR_FIRST_OUT, reg);

		reg = VE_DEC_H265_10BIT_CONFIGURE_FIRST_2BIT_STRIDE(stride);
		cedrus_write(dev, VE_DEC_H265_10BIT_CONFIGURE, reg);
	}

	/* Enable appropriate interruptions. */
	cedrus_write(dev, VE_DEC_H265_CTRL, VE_DEC_H265_CTRL_IRQ_MASK);

	return 0;
}

static int cedrus_h265_start(struct cedrus_ctx *ctx)
{
	struct cedrus_dev *dev = ctx->dev;

	/* Buffer is never accessed by CPU, so we can skip kernel mapping. */
	ctx->codec.h265.neighbor_info_buf =
		dma_alloc_attrs(dev->dev, CEDRUS_H265_NEIGHBOR_INFO_BUF_SIZE,
				&ctx->codec.h265.neighbor_info_buf_addr,
				GFP_KERNEL, DMA_ATTR_NO_KERNEL_MAPPING);
	if (!ctx->codec.h265.neighbor_info_buf)
		return -ENOMEM;

	ctx->codec.h265.entry_points_buf =
		dma_alloc_coherent(dev->dev, CEDRUS_H265_ENTRY_POINTS_BUF_SIZE,
				   &ctx->codec.h265.entry_points_buf_addr,
				   GFP_KERNEL);
	if (!ctx->codec.h265.entry_points_buf) {
		dma_free_attrs(dev->dev, CEDRUS_H265_NEIGHBOR_INFO_BUF_SIZE,
			       ctx->codec.h265.neighbor_info_buf,
			       ctx->codec.h265.neighbor_info_buf_addr,
			       DMA_ATTR_NO_KERNEL_MAPPING);
		return -ENOMEM;
	}

	return 0;
}

static void cedrus_h265_stop(struct cedrus_ctx *ctx)
{
	struct cedrus_dev *dev = ctx->dev;
	struct cedrus_buffer *buf;
	struct vb2_queue *vq;
	unsigned int i;

	vq = v4l2_m2m_get_vq(ctx->fh.m2m_ctx, V4L2_BUF_TYPE_VIDEO_CAPTURE);

	for (i = 0; i < vq->num_buffers; i++) {
		buf = vb2_to_cedrus_buffer(vb2_get_buffer(vq, i));

		if (buf->codec.h265.mv_col_buf_size > 0) {
			dma_free_attrs(dev->dev,
				       buf->codec.h265.mv_col_buf_size,
				       buf->codec.h265.mv_col_buf,
				       buf->codec.h265.mv_col_buf_dma,
				       DMA_ATTR_NO_KERNEL_MAPPING);

			buf->codec.h265.mv_col_buf_size = 0;
		}
	}

	dma_free_attrs(dev->dev, CEDRUS_H265_NEIGHBOR_INFO_BUF_SIZE,
		       ctx->codec.h265.neighbor_info_buf,
		       ctx->codec.h265.neighbor_info_buf_addr,
		       DMA_ATTR_NO_KERNEL_MAPPING);
	dma_free_coherent(dev->dev, CEDRUS_H265_ENTRY_POINTS_BUF_SIZE,
			  ctx->codec.h265.entry_points_buf,
			  ctx->codec.h265.entry_points_buf_addr);
}

static void cedrus_h265_trigger(struct cedrus_ctx *ctx)
{
	struct cedrus_dev *dev = ctx->dev;

	cedrus_write(dev, VE_DEC_H265_TRIGGER, VE_DEC_H265_TRIGGER_DEC_SLICE);
}

static unsigned int cedrus_h265_extra_cap_size(struct cedrus_ctx *ctx,
					       struct v4l2_pix_format *pix_fmt)
{
	if (ctx->bit_depth > 8)
		return cedrus_h265_2bit_size(pix_fmt->width, pix_fmt->height);

	return 0;
}

struct cedrus_dec_ops cedrus_dec_ops_h265 = {
	.irq_clear	= cedrus_h265_irq_clear,
	.irq_disable	= cedrus_h265_irq_disable,
	.irq_status	= cedrus_h265_irq_status,
	.setup		= cedrus_h265_setup,
	.start		= cedrus_h265_start,
	.stop		= cedrus_h265_stop,
	.trigger	= cedrus_h265_trigger,
	.extra_cap_size	= cedrus_h265_extra_cap_size,
};
