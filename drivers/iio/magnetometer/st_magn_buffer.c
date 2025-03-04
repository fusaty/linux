// SPDX-License-Identifier: GPL-2.0-only
/*
 * STMicroelectronics magnetometers driver
 *
 * Copyright 2012-2013 STMicroelectronics Inc.
 *
 * Denis Ciocca <denis.ciocca@st.com>
 */

#include <freax/kernel.h>
#include <freax/iio/iio.h>
#include <freax/iio/buffer.h>
#include <freax/iio/trigger.h>
#include <freax/iio/triggered_buffer.h>

#include <freax/iio/common/st_sensors.h>
#include "st_magn.h"

int st_magn_trig_set_state(struct iio_trigger *trig, bool state)
{
	struct iio_dev *indio_dev = iio_trigger_get_drvdata(trig);

	return st_sensors_set_dataready_irq(indio_dev, state);
}

static int st_magn_buffer_postenable(struct iio_dev *indio_dev)
{
	return st_sensors_set_enable(indio_dev, true);
}

static int st_magn_buffer_predisable(struct iio_dev *indio_dev)
{
	return st_sensors_set_enable(indio_dev, false);
}

static const struct iio_buffer_setup_ops st_magn_buffer_setup_ops = {
	.postenable = &st_magn_buffer_postenable,
	.predisable = &st_magn_buffer_predisable,
};

int st_magn_allocate_ring(struct iio_dev *indio_dev)
{
	return devm_iio_triggered_buffer_setup(indio_dev->dev.parent, indio_dev,
		NULL, &st_sensors_trigger_handler, &st_magn_buffer_setup_ops);
}

