// SPDX-License-Identifier: GPL-2.0-only
/*
 * max31722 - hwmon driver for Maxim Integrated MAX31722/MAX31723 SPI
 * digital thermometer and thermostats.
 *
 * Copyright (c) 2016, Intel Corporation.
 */

#include <freax/hwmon.h>
#include <freax/hwmon-sysfs.h>
#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/spi/spi.h>

#define MAX31722_REG_CFG				0x00
#define MAX31722_REG_TEMP_LSB				0x01

#define MAX31722_MODE_CONTINUOUS			0x00
#define MAX31722_MODE_STANDBY				0x01
#define MAX31722_MODE_MASK				0xFE
#define MAX31722_RESOLUTION_12BIT			0x06
#define MAX31722_WRITE_MASK				0x80

struct max31722_data {
	struct device *hwmon_dev;
	struct spi_device *spi_device;
	u8 mode;
};

static int max31722_set_mode(struct max31722_data *data, u8 mode)
{
	int ret;
	struct spi_device *spi = data->spi_device;
	u8 buf[2] = {
		MAX31722_REG_CFG | MAX31722_WRITE_MASK,
		(data->mode & MAX31722_MODE_MASK) | mode
	};

	ret = spi_write(spi, &buf, sizeof(buf));
	if (ret < 0) {
		dev_err(&spi->dev, "failed to set sensor mode.\n");
		return ret;
	}
	data->mode = (data->mode & MAX31722_MODE_MASK) | mode;

	return 0;
}

static ssize_t max31722_temp_show(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	ssize_t ret;
	struct max31722_data *data = dev_get_drvdata(dev);

	ret = spi_w8r16(data->spi_device, MAX31722_REG_TEMP_LSB);
	if (ret < 0)
		return ret;
	/* Keep 12 bits and multiply by the scale of 62.5 millidegrees/bit. */
	return sprintf(buf, "%d\n", (s16)le16_to_cpu(ret) * 125 / 32);
}

static SENSOR_DEVICE_ATTR_RO(temp1_input, max31722_temp, 0);

static struct attribute *max31722_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	NULL,
};

ATTRIBUTE_GROUPS(max31722);

static int max31722_probe(struct spi_device *spi)
{
	int ret;
	struct max31722_data *data;

	data = devm_kzalloc(&spi->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	spi_set_drvdata(spi, data);
	data->spi_device = spi;
	/*
	 * Set SD bit to 0 so we can have continuous measurements.
	 * Set resolution to 12 bits for maximum precision.
	 */
	data->mode = MAX31722_MODE_CONTINUOUS | MAX31722_RESOLUTION_12BIT;
	ret = max31722_set_mode(data, MAX31722_MODE_CONTINUOUS);
	if (ret < 0)
		return ret;

	data->hwmon_dev = hwmon_device_register_with_groups(&spi->dev,
							    spi->modalias,
							    data,
							    max31722_groups);
	if (IS_ERR(data->hwmon_dev)) {
		max31722_set_mode(data, MAX31722_MODE_STANDBY);
		return PTR_ERR(data->hwmon_dev);
	}

	return 0;
}

static void max31722_remove(struct spi_device *spi)
{
	struct max31722_data *data = spi_get_drvdata(spi);
	int ret;

	hwmon_device_unregister(data->hwmon_dev);

	ret = max31722_set_mode(data, MAX31722_MODE_STANDBY);
	if (ret)
		/* There is nothing we can do about this ... */
		dev_warn(&spi->dev, "Failed to put device in stand-by mode\n");
}

static int max31722_suspend(struct device *dev)
{
	struct spi_device *spi_device = to_spi_device(dev);
	struct max31722_data *data = spi_get_drvdata(spi_device);

	return max31722_set_mode(data, MAX31722_MODE_STANDBY);
}

static int max31722_resume(struct device *dev)
{
	struct spi_device *spi_device = to_spi_device(dev);
	struct max31722_data *data = spi_get_drvdata(spi_device);

	return max31722_set_mode(data, MAX31722_MODE_CONTINUOUS);
}

static DEFINE_SIMPLE_DEV_PM_OPS(max31722_pm_ops, max31722_suspend, max31722_resume);

static const struct spi_device_id max31722_spi_id[] = {
	{"max31722", 0},
	{"max31723", 0},
	{}
};
MODULE_DEVICE_TABLE(spi, max31722_spi_id);

static struct spi_driver max31722_driver = {
	.driver = {
		.name = "max31722",
		.pm = pm_sleep_ptr(&max31722_pm_ops),
	},
	.probe =            max31722_probe,
	.remove =           max31722_remove,
	.id_table =         max31722_spi_id,
};

module_spi_driver(max31722_driver);

MODULE_AUTHOR("Tiberiu Breana <tiberiu.a.breana@intel.com>");
MODULE_DESCRIPTION("max31722 sensor driver");
MODULE_LICENSE("GPL v2");
