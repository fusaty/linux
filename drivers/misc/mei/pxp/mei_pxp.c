// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright © 2020 - 2021 Intel Corporation
 */

/**
 * DOC: MEI_PXP Client Driver
 *
 * The mei_pxp driver acts as a translation layer between PXP
 * protocol  implementer (I915) and ME FW by translating PXP
 * negotiation messages to ME FW command payloads and vice versa.
 */

#include <freax/delay.h>
#include <freax/module.h>
#include <freax/slab.h>
#include <freax/mei.h>
#include <freax/mei_cl_bus.h>
#include <freax/component.h>
#include <drm/drm_connector.h>
#include <drm/i915_component.h>
#include <drm/i915_pxp_tee_interface.h>

#include "mei_pxp.h"

static inline int mei_pxp_reenable(const struct device *dev, struct mei_cl_device *cldev)
{
	int ret;

	dev_warn(dev, "Trying to reset the channel...\n");
	ret = mei_cldev_disable(cldev);
	if (ret < 0)
		dev_warn(dev, "mei_cldev_disable failed. %d\n", ret);
	/*
	 * Explicitly ignoring disable failure,
	 * enable may fix the states and succeed
	 */
	ret = mei_cldev_enable(cldev);
	if (ret < 0)
		dev_err(dev, "mei_cldev_enable failed. %d\n", ret);
	return ret;
}

/**
 * mei_pxp_send_message() - Sends a PXP message to ME FW.
 * @dev: device corresponding to the mei_cl_device
 * @message: a message buffer to send
 * @size: size of the message
 * @timeout_ms: timeout in milliseconds, zero means wait indefinitely.
 *
 * Returns: 0 on Success, <0 on Failure with the following defined failures.
 *         -ENODEV: Client was not connected.
 *                  Caller may attempt to try again immediately.
 *         -ENOMEM: Internal memory allocation failure experienced.
 *                  Caller may sleep to allow kernel reclaim before retrying.
 *         -EINTR : Calling thread received a signal. Caller may choose
 *                  to abandon with the same thread id.
 *         -ETIME : Request is timed out.
 *                  Caller may attempt to try again immediately.
 */
static int
mei_pxp_send_message(struct device *dev, const void *message, size_t size, unsigned long timeout_ms)
{
	struct mei_cl_device *cldev;
	ssize_t byte;
	int ret;

	if (!dev || !message)
		return -EINVAL;

	cldev = to_mei_cl_device(dev);

	byte = mei_cldev_send_timeout(cldev, message, size, timeout_ms);
	if (byte < 0) {
		dev_dbg(dev, "mei_cldev_send failed. %zd\n", byte);
		switch (byte) {
		case -ENOMEM:
			fallthrough;
		case -ENODEV:
			fallthrough;
		case -ETIME:
			ret = mei_pxp_reenable(dev, cldev);
			if (ret)
				byte = ret;
			break;
		}
	}

	return byte;
}

/**
 * mei_pxp_receive_message() - Receives a PXP message from ME FW.
 * @dev: device corresponding to the mei_cl_device
 * @buffer: a message buffer to contain the received message
 * @size: size of the buffer
 * @timeout_ms: timeout in milliseconds, zero means wait indefinitely.
 *
 * Returns: number of bytes send on Success, <0 on Failure with the following defined failures.
 *         -ENODEV: Client was not connected.
 *                  Caller may attempt to try again from send immediately.
 *         -ENOMEM: Internal memory allocation failure experienced.
 *                  Caller may sleep to allow kernel reclaim before retrying.
 *         -EINTR : Calling thread received a signal. Caller will need to repeat calling
 *                  (with a different owning thread) to retrieve existing unclaimed response
 *                  (and may discard it).
 *         -ETIME : Request is timed out.
 *                  Caller may attempt to try again from send immediately.
 */
static int
mei_pxp_receive_message(struct device *dev, void *buffer, size_t size, unsigned long timeout_ms)
{
	struct mei_cl_device *cldev;
	ssize_t byte;
	bool retry = false;
	int ret;

	if (!dev || !buffer)
		return -EINVAL;

	cldev = to_mei_cl_device(dev);

retry:
	byte = mei_cldev_recv_timeout(cldev, buffer, size, timeout_ms);
	if (byte < 0) {
		dev_dbg(dev, "mei_cldev_recv failed. %zd\n", byte);
		switch (byte) {
		case -ENOMEM:
			/* Retry the read when pages are reclaimed */
			msleep(20);
			if (!retry) {
				retry = true;
				goto retry;
			}
			fallthrough;
		case -ENODEV:
			fallthrough;
		case -ETIME:
			ret = mei_pxp_reenable(dev, cldev);
			if (ret)
				byte = ret;
			break;
		}
	}

	return byte;
}

/**
 * mei_pxp_gsc_command() - sends a gsc command, by sending
 * a sgl mei message to gsc and receiving reply from gsc
 *
 * @dev: device corresponding to the mei_cl_device
 * @client_id: client id to send the command to
 * @fence_id: fence id to send the command to
 * @sg_in: scatter gather list containing addresses for rx message buffer
 * @total_in_len: total length of data in 'in' sg, can be less than the sum of buffers sizes
 * @sg_out: scatter gather list containing addresses for tx message buffer
 *
 * Return: bytes sent on Success, <0 on Failure
 */
static ssize_t mei_pxp_gsc_command(struct device *dev, u8 client_id, u32 fence_id,
				   struct scatterlist *sg_in, size_t total_in_len,
				   struct scatterlist *sg_out)
{
	struct mei_cl_device *cldev;

	cldev = to_mei_cl_device(dev);

	return mei_cldev_send_gsc_command(cldev, client_id, fence_id, sg_in, total_in_len, sg_out);
}

static const struct i915_pxp_component_ops mei_pxp_ops = {
	.owner = THIS_MODULE,
	.send = mei_pxp_send_message,
	.recv = mei_pxp_receive_message,
	.gsc_command = mei_pxp_gsc_command,
};

static int mei_component_master_bind(struct device *dev)
{
	struct mei_cl_device *cldev = to_mei_cl_device(dev);
	struct i915_pxp_component *comp_master = mei_cldev_get_drvdata(cldev);
	int ret;

	comp_master->ops = &mei_pxp_ops;
	comp_master->tee_dev = dev;
	ret = component_bind_all(dev, comp_master);
	if (ret < 0)
		return ret;

	return 0;
}

static void mei_component_master_unbind(struct device *dev)
{
	struct mei_cl_device *cldev = to_mei_cl_device(dev);
	struct i915_pxp_component *comp_master = mei_cldev_get_drvdata(cldev);

	component_unbind_all(dev, comp_master);
}

static const struct component_master_ops mei_component_master_ops = {
	.bind = mei_component_master_bind,
	.unbind = mei_component_master_unbind,
};

/**
 * mei_pxp_component_match - compare function for matching mei pxp.
 *
 *    The function checks if the driver is i915, the subcomponent is PXP
 *    and the grand parent of pxp and the parent of i915 are the same
 *    PCH device.
 *
 * @dev: master device
 * @subcomponent: subcomponent to match (I915_COMPONENT_PXP)
 * @data: compare data (mei pxp device)
 *
 * Return:
 * * 1 - if components match
 * * 0 - otherwise
 */
static int mei_pxp_component_match(struct device *dev, int subcomponent,
				   void *data)
{
	struct device *base = data;

	if (!dev)
		return 0;

	if (!dev->driver || strcmp(dev->driver->name, "i915") ||
	    subcomponent != I915_COMPONENT_PXP)
		return 0;

	base = base->parent;
	if (!base) /* mei device */
		return 0;

	base = base->parent; /* pci device */
	/* for dgfx */
	if (base && dev == base)
		return 1;

	/* for pch */
	dev = dev->parent;
	return (base && dev && dev == base);
}

static int mei_pxp_probe(struct mei_cl_device *cldev,
			 const struct mei_cl_device_id *id)
{
	struct i915_pxp_component *comp_master;
	struct component_match *master_match;
	int ret;

	ret = mei_cldev_enable(cldev);
	if (ret < 0) {
		dev_err(&cldev->dev, "mei_cldev_enable Failed. %d\n", ret);
		goto enable_err_exit;
	}

	comp_master = kzalloc(sizeof(*comp_master), GFP_KERNEL);
	if (!comp_master) {
		ret = -ENOMEM;
		goto err_exit;
	}

	master_match = NULL;
	component_match_add_typed(&cldev->dev, &master_match,
				  mei_pxp_component_match, &cldev->dev);
	if (IS_ERR_OR_NULL(master_match)) {
		ret = -ENOMEM;
		goto err_exit;
	}

	mei_cldev_set_drvdata(cldev, comp_master);
	ret = component_master_add_with_match(&cldev->dev,
					      &mei_component_master_ops,
					      master_match);
	if (ret < 0) {
		dev_err(&cldev->dev, "Master comp add failed %d\n", ret);
		goto err_exit;
	}

	return 0;

err_exit:
	mei_cldev_set_drvdata(cldev, NULL);
	kfree(comp_master);
	mei_cldev_disable(cldev);
enable_err_exit:
	return ret;
}

static void mei_pxp_remove(struct mei_cl_device *cldev)
{
	struct i915_pxp_component *comp_master = mei_cldev_get_drvdata(cldev);
	int ret;

	component_master_del(&cldev->dev, &mei_component_master_ops);
	kfree(comp_master);
	mei_cldev_set_drvdata(cldev, NULL);

	ret = mei_cldev_disable(cldev);
	if (ret)
		dev_warn(&cldev->dev, "mei_cldev_disable() failed\n");
}

/* fbf6fcf1-96cf-4e2e-a6a6-1bab8cbe36b1 : PAVP GUID*/
#define MEI_GUID_PXP UUID_LE(0xfbf6fcf1, 0x96cf, 0x4e2e, 0xA6, \
			     0xa6, 0x1b, 0xab, 0x8c, 0xbe, 0x36, 0xb1)

static struct mei_cl_device_id mei_pxp_tbl[] = {
	{ .uuid = MEI_GUID_PXP, .version = MEI_CL_VERSION_ANY },
	{ }
};
MODULE_DEVICE_TABLE(mei, mei_pxp_tbl);

static struct mei_cl_driver mei_pxp_driver = {
	.id_table = mei_pxp_tbl,
	.name = KBUILD_MODNAME,
	.probe = mei_pxp_probe,
	.remove	= mei_pxp_remove,
};

module_mei_cl_driver(mei_pxp_driver);

MODULE_AUTHOR("Intel Corporation");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MEI PXP");
