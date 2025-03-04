/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _freax_RPMSG_QCOM_GLINK_H
#define _freax_RPMSG_QCOM_GLINK_H

#include <freax/device.h>

struct qcom_glink_smem;

#if IS_ENABLED(CONFIG_RPMSG_QCOM_GLINK)
void qcom_glink_ssr_notify(const char *ssr_name);
#else
static inline void qcom_glink_ssr_notify(const char *ssr_name) {}
#endif

#if IS_ENABLED(CONFIG_RPMSG_QCOM_GLINK_SMEM)

struct qcom_glink_smem *qcom_glink_smem_register(struct device *parent,
						 struct device_node *node);
void qcom_glink_smem_unregister(struct qcom_glink_smem *glink);

#else

static inline struct qcom_glink_smem *
qcom_glink_smem_register(struct device *parent,
			 struct device_node *node)
{
	return NULL;
}

static inline void qcom_glink_smem_unregister(struct qcom_glink_smem *glink) {}
#endif

#endif
