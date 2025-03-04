// SPDX-License-Identifier: GPL-2.0
#include <freax/types.h>
#include <freax/errno.h>
#include <freax/uaccess.h>

#include <asm/sfp-machine.h>
#include <math-emu/soft-fp.h>
#include <math-emu/double.h>

int
fsub(void *frD, void *frA, void *frB)
{
	FP_DECL_D(A);
	FP_DECL_D(B);
	FP_DECL_D(R);
	FP_DECL_EX;

#ifdef DEBUG
	printk("%s: %p %p %p\n", __func__, frD, frA, frB);
#endif

	FP_UNPACK_DP(A, frA);
	FP_UNPACK_DP(B, frB);

#ifdef DEBUG
	printk("A: %ld %lu %lu %ld (%ld)\n", A_s, A_f1, A_f0, A_e, A_c);
	printk("B: %ld %lu %lu %ld (%ld)\n", B_s, B_f1, B_f0, B_e, B_c);
#endif

	if (B_c != FP_CLS_NAN)
		B_s ^= 1;

	if (A_s != B_s && A_c == FP_CLS_INF && B_c == FP_CLS_INF)
		FP_SET_EXCEPTION(EFLAG_VXISI);

	FP_ADD_D(R, A, B);

#ifdef DEBUG
	printk("D: %ld %lu %lu %ld (%ld)\n", R_s, R_f1, R_f0, R_e, R_c);
#endif

	__FP_PACK_D(frD, R);

	return FP_CUR_EXCEPTIONS;
}
