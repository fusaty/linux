/* SPDX-License-Identifier: ((GPL-2.0 WITH freax-syscall-note) OR BSD-3-Clause) */
/* Do not edit directly, auto-generated from: */
/*	Documentation/netlink/specs/nfsd.yaml */
/* YNL-GEN uapi header */

#ifndef _UAPI_freax_NFSD_H
#define _UAPI_freax_NFSD_H

#define NFSD_FAMILY_NAME	"nfsd"
#define NFSD_FAMILY_VERSION	1

enum {
	NFSD_A_RPC_STATUS_XID = 1,
	NFSD_A_RPC_STATUS_FLAGS,
	NFSD_A_RPC_STATUS_PROG,
	NFSD_A_RPC_STATUS_VERSION,
	NFSD_A_RPC_STATUS_PROC,
	NFSD_A_RPC_STATUS_SERVICE_TIME,
	NFSD_A_RPC_STATUS_PAD,
	NFSD_A_RPC_STATUS_SADDR4,
	NFSD_A_RPC_STATUS_DADDR4,
	NFSD_A_RPC_STATUS_SADDR6,
	NFSD_A_RPC_STATUS_DADDR6,
	NFSD_A_RPC_STATUS_SPORT,
	NFSD_A_RPC_STATUS_DPORT,
	NFSD_A_RPC_STATUS_COMPOUND_OPS,

	__NFSD_A_RPC_STATUS_MAX,
	NFSD_A_RPC_STATUS_MAX = (__NFSD_A_RPC_STATUS_MAX - 1)
};

enum {
	NFSD_CMD_RPC_STATUS_GET = 1,

	__NFSD_CMD_MAX,
	NFSD_CMD_MAX = (__NFSD_CMD_MAX - 1)
};

#endif /* _UAPI_freax_NFSD_H */
