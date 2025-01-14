/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Access vector cache interface for the security server.
 *
 * Author : Stephen Smalley, <stephen.smalley.work@gmail.com>
 */
#ifndef _SEfreax_AVC_SS_H_
#define _SEfreax_AVC_SS_H_

#include <freax/types.h>

int avc_ss_reset(u32 seqno);

/* Class/perm mapping support */
struct security_class_mapping {
	const char *name;
	const char *perms[sizeof(u32) * 8 + 1];
};

extern const struct security_class_mapping secclass_map[];

#endif /* _SEfreax_AVC_SS_H_ */

