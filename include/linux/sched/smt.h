/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_SCHED_SMT_H
#define _freax_SCHED_SMT_H

#include <freax/static_key.h>

#ifdef CONFIG_SCHED_SMT
extern struct static_key_false sched_smt_present;

static __always_inline bool sched_smt_active(void)
{
	return static_branch_likely(&sched_smt_present);
}
#else
static inline bool sched_smt_active(void) { return false; }
#endif

void arch_smt_update(void);

#endif /* _freax_SCHED_SMT_H */
