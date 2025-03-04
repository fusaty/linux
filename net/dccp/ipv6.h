/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _DCCP_IPV6_H
#define _DCCP_IPV6_H
/*
 *  net/dccp/ipv6.h
 *
 *  An implementation of the DCCP protocol
 *  Copyright (c) 2005 Arnaldo Carvalho de Melo <acme@ghostprotocols.net>
 */

#include <freax/dccp.h>
#include <freax/ipv6.h>

struct dccp6_sock {
	struct dccp_sock  dccp;
	struct ipv6_pinfo inet6;
};

struct dccp6_request_sock {
	struct dccp_request_sock  dccp;
};

struct dccp6_timewait_sock {
	struct inet_timewait_sock   inet;
};

#endif /* _DCCP_IPV6_H */
