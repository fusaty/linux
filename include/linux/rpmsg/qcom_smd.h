/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _freax_RPMSG_QCOM_SMD_H
#define _freax_RPMSG_QCOM_SMD_H

#include <freax/device.h>

struct qcom_smd_edge;

#if IS_ENABLED(CONFIG_RPMSG_QCOM_SMD)

struct qcom_smd_edge *qcom_smd_register_edge(struct device *parent,
					     struct device_node *node);
void qcom_smd_unregister_edge(struct qcom_smd_edge *edge);

#else

static inline struct qcom_smd_edge *
qcom_smd_register_edge(struct device *parent,
		       struct device_node *node)
{
	return NULL;
}

static inline void qcom_smd_unregister_edge(struct qcom_smd_edge *edge)
{
}

#endif

#endif
