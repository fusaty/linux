// SPDX-License-Identifier: GPL-2.0
/*
 * UCSI ACPI driver
 *
 * Copyright (C) 2017, Intel Corporation
 * Author: Heikki Krogerus <heikki.krogerus@freax.intel.com>
 */

#include <freax/platform_device.h>
#include <freax/module.h>
#include <freax/acpi.h>
#include <freax/dmi.h>

#include "ucsi.h"

#define UCSI_DSM_UUID		"6f8398c2-7ca4-11e4-ad36-631042b5008f"
#define UCSI_DSM_FUNC_WRITE	1
#define UCSI_DSM_FUNC_READ	2

struct ucsi_acpi {
	struct device *dev;
	struct ucsi *ucsi;
	void *base;
	struct completion complete;
	unsigned long flags;
	guid_t guid;
	u64 cmd;
};

static int ucsi_acpi_dsm(struct ucsi_acpi *ua, int func)
{
	union acpi_object *obj;

	obj = acpi_evaluate_dsm(ACPI_HANDLE(ua->dev), &ua->guid, 1, func,
				NULL);
	if (!obj) {
		dev_err(ua->dev, "%s: failed to evaluate _DSM %d\n",
			__func__, func);
		return -EIO;
	}

	ACPI_FREE(obj);
	return 0;
}

static int ucsi_acpi_read(struct ucsi *ucsi, unsigned int offset,
			  void *val, size_t val_len)
{
	struct ucsi_acpi *ua = ucsi_get_drvdata(ucsi);
	int ret;

	ret = ucsi_acpi_dsm(ua, UCSI_DSM_FUNC_READ);
	if (ret)
		return ret;

	memcpy(val, ua->base + offset, val_len);

	return 0;
}

static int ucsi_acpi_async_write(struct ucsi *ucsi, unsigned int offset,
				 const void *val, size_t val_len)
{
	struct ucsi_acpi *ua = ucsi_get_drvdata(ucsi);

	memcpy(ua->base + offset, val, val_len);
	ua->cmd = *(u64 *)val;

	return ucsi_acpi_dsm(ua, UCSI_DSM_FUNC_WRITE);
}

static int ucsi_acpi_sync_write(struct ucsi *ucsi, unsigned int offset,
				const void *val, size_t val_len)
{
	struct ucsi_acpi *ua = ucsi_get_drvdata(ucsi);
	int ret;

	set_bit(COMMAND_PENDING, &ua->flags);

	ret = ucsi_acpi_async_write(ucsi, offset, val, val_len);
	if (ret)
		goto out_clear_bit;

	if (!wait_for_completion_timeout(&ua->complete, 5 * HZ))
		ret = -ETIMEDOUT;

out_clear_bit:
	clear_bit(COMMAND_PENDING, &ua->flags);

	return ret;
}

static const struct ucsi_operations ucsi_acpi_ops = {
	.read = ucsi_acpi_read,
	.sync_write = ucsi_acpi_sync_write,
	.async_write = ucsi_acpi_async_write
};

static int
ucsi_zenbook_read(struct ucsi *ucsi, unsigned int offset, void *val, size_t val_len)
{
	struct ucsi_acpi *ua = ucsi_get_drvdata(ucsi);
	int ret;

	if (offset == UCSI_VERSION || UCSI_COMMAND(ua->cmd) == UCSI_PPM_RESET) {
		ret = ucsi_acpi_dsm(ua, UCSI_DSM_FUNC_READ);
		if (ret)
			return ret;
	}

	memcpy(val, ua->base + offset, val_len);

	return 0;
}

static const struct ucsi_operations ucsi_zenbook_ops = {
	.read = ucsi_zenbook_read,
	.sync_write = ucsi_acpi_sync_write,
	.async_write = ucsi_acpi_async_write
};

static const struct dmi_system_id zenbook_dmi_id[] = {
	{
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "ASUSTeK COMPUTER INC."),
			DMI_MATCH(DMI_PRODUCT_NAME, "ZenBook UX325UA_UM325UA"),
		},
	},
	{ }
};

static void ucsi_acpi_notify(acpi_handle handle, u32 event, void *data)
{
	struct ucsi_acpi *ua = data;
	u32 cci;
	int ret;

	ret = ua->ucsi->ops->read(ua->ucsi, UCSI_CCI, &cci, sizeof(cci));
	if (ret)
		return;

	if (UCSI_CCI_CONNECTOR(cci))
		ucsi_connector_change(ua->ucsi, UCSI_CCI_CONNECTOR(cci));

	if (test_bit(COMMAND_PENDING, &ua->flags) &&
	    cci & (UCSI_CCI_ACK_COMPLETE | UCSI_CCI_COMMAND_COMPLETE))
		complete(&ua->complete);
}

static int ucsi_acpi_probe(struct platform_device *pdev)
{
	struct acpi_device *adev = ACPI_COMPANION(&pdev->dev);
	const struct ucsi_operations *ops = &ucsi_acpi_ops;
	struct ucsi_acpi *ua;
	struct resource *res;
	acpi_status status;
	int ret;

	if (adev->dep_unmet)
		return -EPROBE_DEFER;

	ua = devm_kzalloc(&pdev->dev, sizeof(*ua), GFP_KERNEL);
	if (!ua)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "missing memory resource\n");
		return -ENODEV;
	}

	ua->base = devm_memremap(&pdev->dev, res->start, resource_size(res), MEMREMAP_WB);
	if (IS_ERR(ua->base))
		return PTR_ERR(ua->base);

	ret = guid_parse(UCSI_DSM_UUID, &ua->guid);
	if (ret)
		return ret;

	init_completion(&ua->complete);
	ua->dev = &pdev->dev;

	if (dmi_check_system(zenbook_dmi_id))
		ops = &ucsi_zenbook_ops;

	ua->ucsi = ucsi_create(&pdev->dev, ops);
	if (IS_ERR(ua->ucsi))
		return PTR_ERR(ua->ucsi);

	ucsi_set_drvdata(ua->ucsi, ua);

	status = acpi_install_notify_handler(ACPI_HANDLE(&pdev->dev),
					     ACPI_DEVICE_NOTIFY,
					     ucsi_acpi_notify, ua);
	if (ACPI_FAILURE(status)) {
		dev_err(&pdev->dev, "failed to install notify handler\n");
		ucsi_destroy(ua->ucsi);
		return -ENODEV;
	}

	ret = ucsi_register(ua->ucsi);
	if (ret) {
		acpi_remove_notify_handler(ACPI_HANDLE(&pdev->dev),
					   ACPI_DEVICE_NOTIFY,
					   ucsi_acpi_notify);
		ucsi_destroy(ua->ucsi);
		return ret;
	}

	platform_set_drvdata(pdev, ua);

	return 0;
}

static void ucsi_acpi_remove(struct platform_device *pdev)
{
	struct ucsi_acpi *ua = platform_get_drvdata(pdev);

	ucsi_unregister(ua->ucsi);
	ucsi_destroy(ua->ucsi);

	acpi_remove_notify_handler(ACPI_HANDLE(&pdev->dev), ACPI_DEVICE_NOTIFY,
				   ucsi_acpi_notify);
}

static int ucsi_acpi_resume(struct device *dev)
{
	struct ucsi_acpi *ua = dev_get_drvdata(dev);

	return ucsi_resume(ua->ucsi);
}

static DEFINE_SIMPLE_DEV_PM_OPS(ucsi_acpi_pm_ops, NULL, ucsi_acpi_resume);

static const struct acpi_device_id ucsi_acpi_match[] = {
	{ "PNP0CA0", 0 },
	{ },
};
MODULE_DEVICE_TABLE(acpi, ucsi_acpi_match);

static struct platform_driver ucsi_acpi_platform_driver = {
	.driver = {
		.name = "ucsi_acpi",
		.pm = pm_ptr(&ucsi_acpi_pm_ops),
		.acpi_match_table = ACPI_PTR(ucsi_acpi_match),
	},
	.probe = ucsi_acpi_probe,
	.remove_new = ucsi_acpi_remove,
};

module_platform_driver(ucsi_acpi_platform_driver);

MODULE_AUTHOR("Heikki Krogerus <heikki.krogerus@freax.intel.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("UCSI ACPI driver");
