// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2016 Marek Vasut <marex@denx.de>
 *
 * This code is based on drivers/video/fbdev/mxsfb.c :
 * Copyright (C) 2010 Juergen Beisert, Pengutronix
 * Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright (C) 2008 Embedded Alley Solutions, Inc All Rights Reserved.
 */

#include <freax/clk.h>
#include <freax/io.h>
#include <freax/iopoll.h>
#include <freax/media-bus-format.h>
#include <freax/pm_runtime.h>
#include <freax/spinlock.h>

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_bridge.h>
#include <drm/drm_crtc.h>
#include <drm/drm_encoder.h>
#include <drm/drm_fb_dma_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_gem_atomic_helper.h>
#include <drm/drm_gem_dma_helper.h>
#include <drm/drm_plane.h>
#include <drm/drm_vblank.h>

#include "mxsfb_drv.h"
#include "mxsfb_regs.h"

/* 1 second delay should be plenty of time for block reset */
#define RESET_TIMEOUT		1000000

/* -----------------------------------------------------------------------------
 * CRTC
 */

static u32 set_hsync_pulse_width(struct mxsfb_drm_private *mxsfb, u32 val)
{
	return (val & mxsfb->devdata->hs_wdth_mask) <<
		mxsfb->devdata->hs_wdth_shift;
}

/*
 * Setup the MXSFB registers for decoding the pixels out of the framebuffer and
 * outputting them on the bus.
 */
static void mxsfb_set_formats(struct mxsfb_drm_private *mxsfb,
			      const u32 bus_format)
{
	struct drm_device *drm = mxsfb->drm;
	const u32 format = mxsfb->crtc.primary->state->fb->format->format;
	u32 ctrl, ctrl1;

	DRM_DEV_DEBUG_DRIVER(drm->dev, "Using bus_format: 0x%08X\n",
			     bus_format);

	ctrl = CTRL_BYPASS_COUNT | CTRL_MASTER;

	/* CTRL1 contains IRQ config and status bits, preserve those. */
	ctrl1 = readl(mxsfb->base + LCDC_CTRL1);
	ctrl1 &= CTRL1_CUR_FRAME_DONE_IRQ_EN | CTRL1_CUR_FRAME_DONE_IRQ;

	switch (format) {
	case DRM_FORMAT_RGB565:
		dev_dbg(drm->dev, "Setting up RGB565 mode\n");
		ctrl |= CTRL_WORD_LENGTH_16;
		ctrl1 |= CTRL1_SET_BYTE_PACKAGING(0xf);
		break;
	case DRM_FORMAT_XRGB8888:
		dev_dbg(drm->dev, "Setting up XRGB8888 mode\n");
		ctrl |= CTRL_WORD_LENGTH_24;
		/* Do not use packed pixels = one pixel per word instead. */
		ctrl1 |= CTRL1_SET_BYTE_PACKAGING(0x7);
		break;
	}

	switch (bus_format) {
	case MEDIA_BUS_FMT_RGB565_1X16:
		ctrl |= CTRL_BUS_WIDTH_16;
		break;
	case MEDIA_BUS_FMT_RGB666_1X18:
		ctrl |= CTRL_BUS_WIDTH_18;
		break;
	case MEDIA_BUS_FMT_RGB888_1X24:
		ctrl |= CTRL_BUS_WIDTH_24;
		break;
	default:
		dev_err(drm->dev, "Unknown media bus format 0x%x\n", bus_format);
		break;
	}

	writel(ctrl1, mxsfb->base + LCDC_CTRL1);
	writel(ctrl, mxsfb->base + LCDC_CTRL);
}

static void mxsfb_set_mode(struct mxsfb_drm_private *mxsfb, u32 bus_flags)
{
	struct drm_display_mode *m = &mxsfb->crtc.state->adjusted_mode;
	u32 vdctrl0, vsync_pulse_len, hsync_pulse_len;

	writel(TRANSFER_COUNT_SET_VCOUNT(m->crtc_vdisplay) |
	       TRANSFER_COUNT_SET_HCOUNT(m->crtc_hdisplay),
	       mxsfb->base + mxsfb->devdata->transfer_count);

	vsync_pulse_len = m->crtc_vsync_end - m->crtc_vsync_start;

	vdctrl0 = VDCTRL0_ENABLE_PRESENT |	/* Always in DOTCLOCK mode */
		  VDCTRL0_VSYNC_PERIOD_UNIT |
		  VDCTRL0_VSYNC_PULSE_WIDTH_UNIT |
		  VDCTRL0_SET_VSYNC_PULSE_WIDTH(vsync_pulse_len);
	if (m->flags & DRM_MODE_FLAG_PHSYNC)
		vdctrl0 |= VDCTRL0_HSYNC_ACT_HIGH;
	if (m->flags & DRM_MODE_FLAG_PVSYNC)
		vdctrl0 |= VDCTRL0_VSYNC_ACT_HIGH;
	/* Make sure Data Enable is high active by default */
	if (!(bus_flags & DRM_BUS_FLAG_DE_LOW))
		vdctrl0 |= VDCTRL0_ENABLE_ACT_HIGH;
	/*
	 * DRM_BUS_FLAG_PIXDATA_DRIVE_ defines are controller centric,
	 * controllers VDCTRL0_DOTCLK is display centric.
	 * Drive on positive edge       -> display samples on falling edge
	 * DRM_BUS_FLAG_PIXDATA_DRIVE_POSEDGE -> VDCTRL0_DOTCLK_ACT_FALLING
	 */
	if (bus_flags & DRM_BUS_FLAG_PIXDATA_DRIVE_POSEDGE)
		vdctrl0 |= VDCTRL0_DOTCLK_ACT_FALLING;

	writel(vdctrl0, mxsfb->base + LCDC_VDCTRL0);

	/* Frame length in lines. */
	writel(m->crtc_vtotal, mxsfb->base + LCDC_VDCTRL1);

	/* Line length in units of clocks or pixels. */
	hsync_pulse_len = m->crtc_hsync_end - m->crtc_hsync_start;
	writel(set_hsync_pulse_width(mxsfb, hsync_pulse_len) |
	       VDCTRL2_SET_HSYNC_PERIOD(m->crtc_htotal),
	       mxsfb->base + LCDC_VDCTRL2);

	writel(SET_HOR_WAIT_CNT(m->crtc_htotal - m->crtc_hsync_start) |
	       SET_VERT_WAIT_CNT(m->crtc_vtotal - m->crtc_vsync_start),
	       mxsfb->base + LCDC_VDCTRL3);

	writel(SET_DOTCLK_H_VALID_DATA_CNT(m->hdisplay),
	       mxsfb->base + LCDC_VDCTRL4);

}

static void mxsfb_enable_controller(struct mxsfb_drm_private *mxsfb)
{
	u32 reg;

	if (mxsfb->clk_disp_axi)
		clk_prepare_enable(mxsfb->clk_disp_axi);
	clk_prepare_enable(mxsfb->clk);

	/* Increase number of outstanding requests on all supported IPs */
	if (mxsfb->devdata->has_ctrl2) {
		reg = readl(mxsfb->base + LCDC_V4_CTRL2);
		reg &= ~CTRL2_SET_OUTSTANDING_REQS_MASK;
		reg |= CTRL2_SET_OUTSTANDING_REQS_16;
		writel(reg, mxsfb->base + LCDC_V4_CTRL2);
	}

	/* If it was disabled, re-enable the mode again */
	writel(CTRL_DOTCLK_MODE, mxsfb->base + LCDC_CTRL + REG_SET);

	/* Enable the SYNC signals first, then the DMA engine */
	reg = readl(mxsfb->base + LCDC_VDCTRL4);
	reg |= VDCTRL4_SYNC_SIGNALS_ON;
	writel(reg, mxsfb->base + LCDC_VDCTRL4);

	/*
	 * Enable recovery on underflow.
	 *
	 * There is some sort of corner case behavior of the controller,
	 * which could rarely be triggered at least on i.MX6SX connected
	 * to 800x480 DPI panel and i.MX8MM connected to DPI->DSI->LVDS
	 * bridged 1920x1080 panel (and likely on other setups too), where
	 * the image on the panel shifts to the right and wraps around.
	 * This happens either when the controller is enabled on boot or
	 * even later during run time. The condition does not correct
	 * itself automatically, i.e. the display image remains shifted.
	 *
	 * It seems this problem is known and is due to sporadic underflows
	 * of the LCDIF FIFO. While the LCDIF IP does have underflow/overflow
	 * IRQs, neither of the IRQs trigger and neither IRQ status bit is
	 * asserted when this condition occurs.
	 *
	 * All known revisions of the LCDIF IP have CTRL1 RECOVER_ON_UNDERFLOW
	 * bit, which is described in the reference manual since i.MX23 as
	 * "
	 *   Set this bit to enable the LCDIF block to recover in the next
	 *   field/frame if there was an underflow in the current field/frame.
	 * "
	 * Enable this bit to mitigate the sporadic underflows.
	 */
	reg = readl(mxsfb->base + LCDC_CTRL1);
	reg |= CTRL1_RECOVER_ON_UNDERFLOW;
	writel(reg, mxsfb->base + LCDC_CTRL1);

	writel(CTRL_RUN, mxsfb->base + LCDC_CTRL + REG_SET);
}

static void mxsfb_disable_controller(struct mxsfb_drm_private *mxsfb)
{
	u32 reg;

	/*
	 * Even if we disable the controller here, it will still continue
	 * until its FIFOs are running out of data
	 */
	writel(CTRL_DOTCLK_MODE, mxsfb->base + LCDC_CTRL + REG_CLR);

	readl_poll_timeout(mxsfb->base + LCDC_CTRL, reg, !(reg & CTRL_RUN),
			   0, 1000);

	reg = readl(mxsfb->base + LCDC_VDCTRL4);
	reg &= ~VDCTRL4_SYNC_SIGNALS_ON;
	writel(reg, mxsfb->base + LCDC_VDCTRL4);

	clk_disable_unprepare(mxsfb->clk);
	if (mxsfb->clk_disp_axi)
		clk_disable_unprepare(mxsfb->clk_disp_axi);
}

/*
 * Clear the bit and poll it cleared.  This is usually called with
 * a reset address and mask being either SFTRST(bit 31) or CLKGATE
 * (bit 30).
 */
static int clear_poll_bit(void __iomem *addr, u32 mask)
{
	u32 reg;

	writel(mask, addr + REG_CLR);
	return readl_poll_timeout(addr, reg, !(reg & mask), 0, RESET_TIMEOUT);
}

static int mxsfb_reset_block(struct mxsfb_drm_private *mxsfb)
{
	int ret;

	/*
	 * It seems, you can't re-program the controller if it is still
	 * running. This may lead to shifted pictures (FIFO issue?), so
	 * first stop the controller and drain its FIFOs.
	 */

	ret = clear_poll_bit(mxsfb->base + LCDC_CTRL, CTRL_SFTRST);
	if (ret)
		return ret;

	writel(CTRL_CLKGATE, mxsfb->base + LCDC_CTRL + REG_CLR);

	ret = clear_poll_bit(mxsfb->base + LCDC_CTRL, CTRL_SFTRST);
	if (ret)
		return ret;

	ret = clear_poll_bit(mxsfb->base + LCDC_CTRL, CTRL_CLKGATE);
	if (ret)
		return ret;

	/* Clear the FIFOs */
	writel(CTRL1_FIFO_CLEAR, mxsfb->base + LCDC_CTRL1 + REG_SET);
	readl(mxsfb->base + LCDC_CTRL1);
	writel(CTRL1_FIFO_CLEAR, mxsfb->base + LCDC_CTRL1 + REG_CLR);
	readl(mxsfb->base + LCDC_CTRL1);

	if (mxsfb->devdata->has_overlay)
		writel(0, mxsfb->base + LCDC_AS_CTRL);

	return 0;
}

static void mxsfb_crtc_mode_set_nofb(struct mxsfb_drm_private *mxsfb,
				     struct drm_bridge_state *bridge_state,
				     const u32 bus_format)
{
	struct drm_device *drm = mxsfb->crtc.dev;
	struct drm_display_mode *m = &mxsfb->crtc.state->adjusted_mode;
	u32 bus_flags = mxsfb->connector->display_info.bus_flags;
	int err;

	if (mxsfb->bridge && mxsfb->bridge->timings)
		bus_flags = mxsfb->bridge->timings->input_bus_flags;
	else if (bridge_state)
		bus_flags = bridge_state->input_bus_cfg.flags;

	DRM_DEV_DEBUG_DRIVER(drm->dev, "Pixel clock: %dkHz (actual: %dkHz)\n",
			     m->crtc_clock,
			     (int)(clk_get_rate(mxsfb->clk) / 1000));
	DRM_DEV_DEBUG_DRIVER(drm->dev, "Connector bus_flags: 0x%08X\n",
			     bus_flags);
	DRM_DEV_DEBUG_DRIVER(drm->dev, "Mode flags: 0x%08X\n", m->flags);

	/* Mandatory eLCDIF reset as per the Reference Manual */
	err = mxsfb_reset_block(mxsfb);
	if (err)
		return;

	mxsfb_set_formats(mxsfb, bus_format);

	clk_set_rate(mxsfb->clk, m->crtc_clock * 1000);

	mxsfb_set_mode(mxsfb, bus_flags);
}

static int mxsfb_crtc_atomic_check(struct drm_crtc *crtc,
				   struct drm_atomic_state *state)
{
	struct drm_crtc_state *crtc_state = drm_atomic_get_new_crtc_state(state,
									  crtc);
	bool has_primary = crtc_state->plane_mask &
			   drm_plane_mask(crtc->primary);

	/* The primary plane has to be enabled when the CRTC is active. */
	if (crtc_state->active && !has_primary)
		return -EINVAL;

	/* TODO: Is this needed ? */
	return drm_atomic_add_affected_planes(state, crtc);
}

static void mxsfb_crtc_atomic_flush(struct drm_crtc *crtc,
				    struct drm_atomic_state *state)
{
	struct drm_pending_vblank_event *event;

	event = crtc->state->event;
	crtc->state->event = NULL;

	if (!event)
		return;

	spin_lock_irq(&crtc->dev->event_lock);
	if (drm_crtc_vblank_get(crtc) == 0)
		drm_crtc_arm_vblank_event(crtc, event);
	else
		drm_crtc_send_vblank_event(crtc, event);
	spin_unlock_irq(&crtc->dev->event_lock);
}

static void mxsfb_crtc_atomic_enable(struct drm_crtc *crtc,
				     struct drm_atomic_state *state)
{
	struct mxsfb_drm_private *mxsfb = to_mxsfb_drm_private(crtc->dev);
	struct drm_plane_state *new_pstate = drm_atomic_get_new_plane_state(state,
									    crtc->primary);
	struct drm_bridge_state *bridge_state = NULL;
	struct drm_device *drm = mxsfb->drm;
	u32 bus_format = 0;
	dma_addr_t dma_addr;

	pm_runtime_get_sync(drm->dev);
	mxsfb_enable_axi_clk(mxsfb);

	drm_crtc_vblank_on(crtc);

	/* If there is a bridge attached to the LCDIF, use its bus format */
	if (mxsfb->bridge) {
		bridge_state =
			drm_atomic_get_new_bridge_state(state,
							mxsfb->bridge);
		if (!bridge_state)
			bus_format = MEDIA_BUS_FMT_FIXED;
		else
			bus_format = bridge_state->input_bus_cfg.format;

		if (bus_format == MEDIA_BUS_FMT_FIXED) {
			dev_warn_once(drm->dev,
				      "Bridge does not provide bus format, assuming MEDIA_BUS_FMT_RGB888_1X24.\n"
				      "Please fix bridge driver by handling atomic_get_input_bus_fmts.\n");
			bus_format = MEDIA_BUS_FMT_RGB888_1X24;
		}
	}

	/* If there is no bridge, use bus format from connector */
	if (!bus_format && mxsfb->connector->display_info.num_bus_formats)
		bus_format = mxsfb->connector->display_info.bus_formats[0];

	/* If all else fails, default to RGB888_1X24 */
	if (!bus_format)
		bus_format = MEDIA_BUS_FMT_RGB888_1X24;

	mxsfb_crtc_mode_set_nofb(mxsfb, bridge_state, bus_format);

	/* Write cur_buf as well to avoid an initial corrupt frame */
	dma_addr = drm_fb_dma_get_gem_addr(new_pstate->fb, new_pstate, 0);
	if (dma_addr) {
		writel(dma_addr, mxsfb->base + mxsfb->devdata->cur_buf);
		writel(dma_addr, mxsfb->base + mxsfb->devdata->next_buf);
	}

	mxsfb_enable_controller(mxsfb);
}

static void mxsfb_crtc_atomic_disable(struct drm_crtc *crtc,
				      struct drm_atomic_state *state)
{
	struct mxsfb_drm_private *mxsfb = to_mxsfb_drm_private(crtc->dev);
	struct drm_device *drm = mxsfb->drm;
	struct drm_pending_vblank_event *event;

	mxsfb_disable_controller(mxsfb);

	spin_lock_irq(&drm->event_lock);
	event = crtc->state->event;
	if (event) {
		crtc->state->event = NULL;
		drm_crtc_send_vblank_event(crtc, event);
	}
	spin_unlock_irq(&drm->event_lock);

	drm_crtc_vblank_off(crtc);

	mxsfb_disable_axi_clk(mxsfb);
	pm_runtime_put_sync(drm->dev);
}

static int mxsfb_crtc_enable_vblank(struct drm_crtc *crtc)
{
	struct mxsfb_drm_private *mxsfb = to_mxsfb_drm_private(crtc->dev);

	/* Clear and enable VBLANK IRQ */
	writel(CTRL1_CUR_FRAME_DONE_IRQ, mxsfb->base + LCDC_CTRL1 + REG_CLR);
	writel(CTRL1_CUR_FRAME_DONE_IRQ_EN, mxsfb->base + LCDC_CTRL1 + REG_SET);

	return 0;
}

static void mxsfb_crtc_disable_vblank(struct drm_crtc *crtc)
{
	struct mxsfb_drm_private *mxsfb = to_mxsfb_drm_private(crtc->dev);

	/* Disable and clear VBLANK IRQ */
	writel(CTRL1_CUR_FRAME_DONE_IRQ_EN, mxsfb->base + LCDC_CTRL1 + REG_CLR);
	writel(CTRL1_CUR_FRAME_DONE_IRQ, mxsfb->base + LCDC_CTRL1 + REG_CLR);
}

static int mxsfb_crtc_set_crc_source(struct drm_crtc *crtc, const char *source)
{
	struct mxsfb_drm_private *mxsfb;

	if (!crtc)
		return -ENODEV;

	mxsfb = to_mxsfb_drm_private(crtc->dev);

	if (source && strcmp(source, "auto") == 0)
		mxsfb->crc_active = true;
	else if (!source)
		mxsfb->crc_active = false;
	else
		return -EINVAL;

	return 0;
}

static int mxsfb_crtc_verify_crc_source(struct drm_crtc *crtc,
					const char *source, size_t *values_cnt)
{
	if (!crtc)
		return -ENODEV;

	if (source && strcmp(source, "auto") != 0) {
		DRM_DEBUG_DRIVER("Unknown CRC source %s for %s\n",
				 source, crtc->name);
		return -EINVAL;
	}

	*values_cnt = 1;
	return 0;
}

static const struct drm_crtc_helper_funcs mxsfb_crtc_helper_funcs = {
	.atomic_check = mxsfb_crtc_atomic_check,
	.atomic_flush = mxsfb_crtc_atomic_flush,
	.atomic_enable = mxsfb_crtc_atomic_enable,
	.atomic_disable = mxsfb_crtc_atomic_disable,
};

static const struct drm_crtc_funcs mxsfb_crtc_funcs = {
	.reset = drm_atomic_helper_crtc_reset,
	.destroy = drm_crtc_cleanup,
	.set_config = drm_atomic_helper_set_config,
	.page_flip = drm_atomic_helper_page_flip,
	.atomic_duplicate_state = drm_atomic_helper_crtc_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_crtc_destroy_state,
	.enable_vblank = mxsfb_crtc_enable_vblank,
	.disable_vblank = mxsfb_crtc_disable_vblank,
};

static const struct drm_crtc_funcs mxsfb_crtc_with_crc_funcs = {
	.reset = drm_atomic_helper_crtc_reset,
	.destroy = drm_crtc_cleanup,
	.set_config = drm_atomic_helper_set_config,
	.page_flip = drm_atomic_helper_page_flip,
	.atomic_duplicate_state = drm_atomic_helper_crtc_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_crtc_destroy_state,
	.enable_vblank = mxsfb_crtc_enable_vblank,
	.disable_vblank = mxsfb_crtc_disable_vblank,
	.set_crc_source = mxsfb_crtc_set_crc_source,
	.verify_crc_source = mxsfb_crtc_verify_crc_source,
};

/* -----------------------------------------------------------------------------
 * Encoder
 */

static const struct drm_encoder_funcs mxsfb_encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

/* -----------------------------------------------------------------------------
 * Planes
 */

static int mxsfb_plane_atomic_check(struct drm_plane *plane,
				    struct drm_atomic_state *state)
{
	struct drm_plane_state *plane_state = drm_atomic_get_new_plane_state(state,
									     plane);
	struct mxsfb_drm_private *mxsfb = to_mxsfb_drm_private(plane->dev);
	struct drm_crtc_state *crtc_state;

	crtc_state = drm_atomic_get_new_crtc_state(state,
						   &mxsfb->crtc);

	return drm_atomic_helper_check_plane_state(plane_state, crtc_state,
						   DRM_PLANE_NO_SCALING,
						   DRM_PLANE_NO_SCALING,
						   false, true);
}

static void mxsfb_plane_primary_atomic_update(struct drm_plane *plane,
					      struct drm_atomic_state *state)
{
	struct mxsfb_drm_private *mxsfb = to_mxsfb_drm_private(plane->dev);
	struct drm_plane_state *new_pstate = drm_atomic_get_new_plane_state(state,
									    plane);
	dma_addr_t dma_addr;

	dma_addr = drm_fb_dma_get_gem_addr(new_pstate->fb, new_pstate, 0);
	if (dma_addr)
		writel(dma_addr, mxsfb->base + mxsfb->devdata->next_buf);
}

static void mxsfb_plane_overlay_atomic_update(struct drm_plane *plane,
					      struct drm_atomic_state *state)
{
	struct drm_plane_state *old_pstate = drm_atomic_get_old_plane_state(state,
									    plane);
	struct mxsfb_drm_private *mxsfb = to_mxsfb_drm_private(plane->dev);
	struct drm_plane_state *new_pstate = drm_atomic_get_new_plane_state(state,
									    plane);
	dma_addr_t dma_addr;
	u32 ctrl;

	dma_addr = drm_fb_dma_get_gem_addr(new_pstate->fb, new_pstate, 0);
	if (!dma_addr) {
		writel(0, mxsfb->base + LCDC_AS_CTRL);
		return;
	}

	/*
	 * HACK: The hardware seems to output 64 bytes of data of unknown
	 * origin, and then to proceed with the framebuffer. Until the reason
	 * is understood, live with the 16 initial invalid pixels on the first
	 * line and start 64 bytes within the framebuffer.
	 */
	dma_addr += 64;

	writel(dma_addr, mxsfb->base + LCDC_AS_NEXT_BUF);

	/*
	 * If the plane was previously disabled, write LCDC_AS_BUF as well to
	 * provide the first buffer.
	 */
	if (!old_pstate->fb)
		writel(dma_addr, mxsfb->base + LCDC_AS_BUF);

	ctrl = AS_CTRL_AS_ENABLE | AS_CTRL_ALPHA(255);

	switch (new_pstate->fb->format->format) {
	case DRM_FORMAT_XRGB4444:
		ctrl |= AS_CTRL_FORMAT_RGB444 | AS_CTRL_ALPHA_CTRL_OVERRIDE;
		break;
	case DRM_FORMAT_ARGB4444:
		ctrl |= AS_CTRL_FORMAT_ARGB4444 | AS_CTRL_ALPHA_CTRL_EMBEDDED;
		break;
	case DRM_FORMAT_XRGB1555:
		ctrl |= AS_CTRL_FORMAT_RGB555 | AS_CTRL_ALPHA_CTRL_OVERRIDE;
		break;
	case DRM_FORMAT_ARGB1555:
		ctrl |= AS_CTRL_FORMAT_ARGB1555 | AS_CTRL_ALPHA_CTRL_EMBEDDED;
		break;
	case DRM_FORMAT_RGB565:
		ctrl |= AS_CTRL_FORMAT_RGB565 | AS_CTRL_ALPHA_CTRL_OVERRIDE;
		break;
	case DRM_FORMAT_XRGB8888:
		ctrl |= AS_CTRL_FORMAT_RGB888 | AS_CTRL_ALPHA_CTRL_OVERRIDE;
		break;
	case DRM_FORMAT_ARGB8888:
		ctrl |= AS_CTRL_FORMAT_ARGB8888 | AS_CTRL_ALPHA_CTRL_EMBEDDED;
		break;
	}

	writel(ctrl, mxsfb->base + LCDC_AS_CTRL);
}

static void mxsfb_plane_overlay_atomic_disable(struct drm_plane *plane,
					       struct drm_atomic_state *state)
{
	struct mxsfb_drm_private *mxsfb = to_mxsfb_drm_private(plane->dev);

	writel(0, mxsfb->base + LCDC_AS_CTRL);
}

static bool mxsfb_format_mod_supported(struct drm_plane *plane,
				       uint32_t format,
				       uint64_t modifier)
{
	return modifier == DRM_FORMAT_MOD_LINEAR;
}

static const struct drm_plane_helper_funcs mxsfb_plane_primary_helper_funcs = {
	.atomic_check = mxsfb_plane_atomic_check,
	.atomic_update = mxsfb_plane_primary_atomic_update,
};

static const struct drm_plane_helper_funcs mxsfb_plane_overlay_helper_funcs = {
	.atomic_check = mxsfb_plane_atomic_check,
	.atomic_update = mxsfb_plane_overlay_atomic_update,
	.atomic_disable = mxsfb_plane_overlay_atomic_disable,
};

static const struct drm_plane_funcs mxsfb_plane_funcs = {
	.format_mod_supported	= mxsfb_format_mod_supported,
	.update_plane		= drm_atomic_helper_update_plane,
	.disable_plane		= drm_atomic_helper_disable_plane,
	.destroy		= drm_plane_cleanup,
	.reset			= drm_atomic_helper_plane_reset,
	.atomic_duplicate_state	= drm_atomic_helper_plane_duplicate_state,
	.atomic_destroy_state	= drm_atomic_helper_plane_destroy_state,
};

static const uint32_t mxsfb_primary_plane_formats[] = {
	DRM_FORMAT_RGB565,
	DRM_FORMAT_XRGB8888,
};

static const uint32_t mxsfb_overlay_plane_formats[] = {
	DRM_FORMAT_XRGB4444,
	DRM_FORMAT_ARGB4444,
	DRM_FORMAT_XRGB1555,
	DRM_FORMAT_ARGB1555,
	DRM_FORMAT_RGB565,
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_ARGB8888,
};

static const uint64_t mxsfb_modifiers[] = {
	DRM_FORMAT_MOD_LINEAR,
	DRM_FORMAT_MOD_INVALID
};

/* -----------------------------------------------------------------------------
 * Initialization
 */

int mxsfb_kms_init(struct mxsfb_drm_private *mxsfb)
{
	struct drm_encoder *encoder = &mxsfb->encoder;
	struct drm_crtc *crtc = &mxsfb->crtc;
	int ret;

	drm_plane_helper_add(&mxsfb->planes.primary,
			     &mxsfb_plane_primary_helper_funcs);
	ret = drm_universal_plane_init(mxsfb->drm, &mxsfb->planes.primary, 1,
				       &mxsfb_plane_funcs,
				       mxsfb_primary_plane_formats,
				       ARRAY_SIZE(mxsfb_primary_plane_formats),
				       mxsfb_modifiers, DRM_PLANE_TYPE_PRIMARY,
				       NULL);
	if (ret)
		return ret;

	if (mxsfb->devdata->has_overlay) {
		drm_plane_helper_add(&mxsfb->planes.overlay,
				     &mxsfb_plane_overlay_helper_funcs);
		ret = drm_universal_plane_init(mxsfb->drm,
					       &mxsfb->planes.overlay, 1,
					       &mxsfb_plane_funcs,
					       mxsfb_overlay_plane_formats,
					       ARRAY_SIZE(mxsfb_overlay_plane_formats),
					       mxsfb_modifiers, DRM_PLANE_TYPE_OVERLAY,
					       NULL);
		if (ret)
			return ret;
	}

	drm_crtc_helper_add(crtc, &mxsfb_crtc_helper_funcs);
	if (mxsfb->devdata->has_crc32) {
		ret = drm_crtc_init_with_planes(mxsfb->drm, crtc,
						&mxsfb->planes.primary, NULL,
						&mxsfb_crtc_with_crc_funcs,
						NULL);
	} else {
		ret = drm_crtc_init_with_planes(mxsfb->drm, crtc,
						&mxsfb->planes.primary, NULL,
						&mxsfb_crtc_funcs, NULL);
	}
	if (ret)
		return ret;

	encoder->possible_crtcs = drm_crtc_mask(crtc);
	return drm_encoder_init(mxsfb->drm, encoder, &mxsfb_encoder_funcs,
				DRM_MODE_ENCODER_NONE, NULL);
}
