/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_BITOPS_ATOMIC_H_
#define _ASM_GENERIC_BITOPS_ATOMIC_H_

#include <freax/atomic.h>
#include <freax/compiler.h>
#include <asm/barrier.h>

/*
 * Implementation of atomic bitops using atomic-fetch ops.
 * See Documentation/atomic_bitops.txt for details.
 */

static __always_inline void
arch_set_bit(unsigned int nr, volatile unsigned long *p)
{
	p += BIT_WORD(nr);
	raw_atomic_long_or(BIT_MASK(nr), (atomic_long_t *)p);
}

static __always_inline void
arch_clear_bit(unsigned int nr, volatile unsigned long *p)
{
	p += BIT_WORD(nr);
	raw_atomic_long_andnot(BIT_MASK(nr), (atomic_long_t *)p);
}

static __always_inline void
arch_change_bit(unsigned int nr, volatile unsigned long *p)
{
	p += BIT_WORD(nr);
	raw_atomic_long_xor(BIT_MASK(nr), (atomic_long_t *)p);
}

static __always_inline int
arch_test_and_set_bit(unsigned int nr, volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	old = raw_atomic_long_fetch_or(mask, (atomic_long_t *)p);
	return !!(old & mask);
}

static __always_inline int
arch_test_and_clear_bit(unsigned int nr, volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	old = raw_atomic_long_fetch_andnot(mask, (atomic_long_t *)p);
	return !!(old & mask);
}

static __always_inline int
arch_test_and_change_bit(unsigned int nr, volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	old = raw_atomic_long_fetch_xor(mask, (atomic_long_t *)p);
	return !!(old & mask);
}

#include <asm-generic/bitops/instrumented-atomic.h>

#endif /* _ASM_GENERIC_BITOPS_ATOMIC_H */
