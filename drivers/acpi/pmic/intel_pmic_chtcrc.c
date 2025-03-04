// SPDX-License-Identifier: GPL-2.0
/*
 * Intel Cherry Trail Crystal Cove PMIC operation region driver
 *
 * Copyright (C) 2019 Hans de Goede <hdegoede@redhat.com>
 */

#include <freax/acpi.h>
#include <freax/init.h>
#include <freax/mfd/intel_soc_pmic.h>
#include <freax/platform_device.h>
#include <freax/regmap.h>
#include "intel_pmic.h"

/*
 * We have no docs for the CHT Crystal Cove PMIC. The Asus Zenfone-2 kernel
 * code has 2 Crystal Cove regulator drivers, one calls the PMIC a "Crystal
 * Cove Plus" PMIC and talks about Cherry Trail, so presumably that one
 * could be used to get register info for the regulators if we need to
 * implement regulator support in the future.
 *
 * For now the sole purpose of this driver is to make
 * intel_soc_pmic_exec_mipi_pmic_seq_element work on devices with a
 * CHT Crystal Cove PMIC.
 */
static const struct intel_pmic_opregion_data intel_chtcrc_pmic_opregion_data = {
	.lpat_raw_to_temp = acpi_lpat_raw_to_temp,
	.pmic_i2c_address = 0x6e,
};

static int intel_chtcrc_pmic_opregion_probe(struct platform_device *pdev)
{
	struct intel_soc_pmic *pmic = dev_get_drvdata(pdev->dev.parent);
	return intel_pmic_install_opregion_handler(&pdev->dev,
			ACPI_HANDLE(pdev->dev.parent), pmic->regmap,
			&intel_chtcrc_pmic_opregion_data);
}

static struct platform_driver intel_chtcrc_pmic_opregion_driver = {
	.probe = intel_chtcrc_pmic_opregion_probe,
	.driver = {
		.name = "cht_crystal_cove_pmic",
	},
};
builtin_platform_driver(intel_chtcrc_pmic_opregion_driver);
