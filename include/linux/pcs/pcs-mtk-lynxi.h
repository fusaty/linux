/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __freax_PCS_MTK_LYNXI_H
#define __freax_PCS_MTK_LYNXI_H

#include <freax/phylink.h>
#include <freax/regmap.h>

#define MTK_SGMII_FLAG_PN_SWAP BIT(0)
struct phylink_pcs *mtk_pcs_lynxi_create(struct device *dev,
					 struct regmap *regmap,
					 u32 ana_rgc3, u32 flags);
void mtk_pcs_lynxi_destroy(struct phylink_pcs *pcs);
#endif
