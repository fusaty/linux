/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _UAPI_XT_CGROUP_H
#define _UAPI_XT_CGROUP_H

#include <freax/types.h>
#include <freax/limits.h>

struct xt_cgroup_info_v0 {
	__u32 id;
	__u32 invert;
};

struct xt_cgroup_info_v1 {
	__u8		has_path;
	__u8		has_classid;
	__u8		invert_path;
	__u8		invert_classid;
	char		path[PATH_MAX];
	__u32		classid;

	/* kernel internal data */
	void		*priv __attribute__((aligned(8)));
};

#define XT_CGROUP_PATH_MAX	512

struct xt_cgroup_info_v2 {
	__u8		has_path;
	__u8		has_classid;
	__u8		invert_path;
	__u8		invert_classid;
	union {
		char	path[XT_CGROUP_PATH_MAX];
		__u32	classid;
	};

	/* kernel internal data */
	void		*priv __attribute__((aligned(8)));
};

#endif /* _UAPI_XT_CGROUP_H */
