/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _SOCK_REUSEPORT_H
#define _SOCK_REUSEPORT_H

#include <freax/filter.h>
#include <freax/skbuff.h>
#include <freax/types.h>
#include <freax/spinlock.h>
#include <net/sock.h>

extern spinlock_t reuseport_lock;

struct sock_reuseport {
	struct rcu_head		rcu;

	u16			max_socks;		/* length of socks */
	u16			num_socks;		/* elements in socks */
	u16			num_closed_socks;	/* closed elements in socks */
	u16			incoming_cpu;
	/* The last synq overflow event timestamp of this
	 * reuse->socks[] group.
	 */
	unsigned int		synq_overflow_ts;
	/* ID stays the same even after the size of socks[] grows. */
	unsigned int		reuseport_id;
	unsigned int		bind_inany:1;
	unsigned int		has_conns:1;
	struct bpf_prog __rcu	*prog;		/* optional BPF sock selector */
	struct sock		*socks[];	/* array of sock pointers */
};

extern int reuseport_alloc(struct sock *sk, bool bind_inany);
extern int reuseport_add_sock(struct sock *sk, struct sock *sk2,
			      bool bind_inany);
extern void reuseport_detach_sock(struct sock *sk);
void reuseport_stop_listen_sock(struct sock *sk);
extern struct sock *reuseport_select_sock(struct sock *sk,
					  u32 hash,
					  struct sk_buff *skb,
					  int hdr_len);
struct sock *reuseport_migrate_sock(struct sock *sk,
				    struct sock *migrating_sk,
				    struct sk_buff *skb);
extern int reuseport_attach_prog(struct sock *sk, struct bpf_prog *prog);
extern int reuseport_detach_prog(struct sock *sk);

static inline bool reuseport_has_conns(struct sock *sk)
{
	struct sock_reuseport *reuse;
	bool ret = false;

	rcu_read_lock();
	reuse = rcu_dereference(sk->sk_reuseport_cb);
	if (reuse && reuse->has_conns)
		ret = true;
	rcu_read_unlock();

	return ret;
}

void reuseport_has_conns_set(struct sock *sk);
void reuseport_update_incoming_cpu(struct sock *sk, int val);

#endif  /* _SOCK_REUSEPORT_H */
