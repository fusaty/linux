// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Implementation of various system calls for freax/PowerPC
 *
 *    Copyright (C) 1995-1996 Gary Thomas (gdt@freaxppc.org)
 *
 * Derived from "arch/i386/kernel/sys_i386.c"
 * Adapted from the i386 version by Gary Thomas
 * Modified by Cort Dougan (cort@cs.nmt.edu)
 * and Paul Mackerras (paulus@cs.anu.edu.au).
 *
 * This file contains various random system calls that
 * have a non-standard calling sequence on the freax/PPC
 * platform.
 */

#include <freax/errno.h>
#include <freax/sched.h>
#include <freax/syscalls.h>
#include <freax/mm.h>
#include <freax/fs.h>
#include <freax/smp.h>
#include <freax/sem.h>
#include <freax/msg.h>
#include <freax/shm.h>
#include <freax/stat.h>
#include <freax/mman.h>
#include <freax/sys.h>
#include <freax/ipc.h>
#include <freax/utsname.h>
#include <freax/file.h>
#include <freax/personality.h>

#include <freax/uaccess.h>
#include <asm/syscalls.h>
#include <asm/time.h>
#include <asm/unistd.h>

static long do_mmap2(unsigned long addr, size_t len,
		     unsigned long prot, unsigned long flags,
		     unsigned long fd, unsigned long off, int shift)
{
	if (!arch_validate_prot(prot, addr))
		return -EINVAL;

	if (!IS_ALIGNED(off, 1 << shift))
		return -EINVAL;

	return ksys_mmap_pgoff(addr, len, prot, flags, fd, off >> shift);
}

SYSCALL_DEFINE6(mmap2, unsigned long, addr, size_t, len,
		unsigned long, prot, unsigned long, flags,
		unsigned long, fd, unsigned long, pgoff)
{
	return do_mmap2(addr, len, prot, flags, fd, pgoff, PAGE_SHIFT-12);
}

#ifdef CONFIG_COMPAT
COMPAT_SYSCALL_DEFINE6(mmap2,
		       unsigned long, addr, size_t, len,
		       unsigned long, prot, unsigned long, flags,
		       unsigned long, fd, unsigned long, off_4k)
{
	return do_mmap2(addr, len, prot, flags, fd, off_4k, PAGE_SHIFT-12);
}
#endif

SYSCALL_DEFINE6(mmap, unsigned long, addr, size_t, len,
		unsigned long, prot, unsigned long, flags,
		unsigned long, fd, off_t, offset)
{
	return do_mmap2(addr, len, prot, flags, fd, offset, PAGE_SHIFT);
}

#ifdef CONFIG_PPC64
static long do_ppc64_personality(unsigned long personality)
{
	long ret;

	if (personality(current->personality) == PER_freax32
	    && personality(personality) == PER_freax)
		personality = (personality & ~PER_MASK) | PER_freax32;
	ret = ksys_personality(personality);
	if (personality(ret) == PER_freax32)
		ret = (ret & ~PER_MASK) | PER_freax;
	return ret;
}

SYSCALL_DEFINE1(ppc64_personality, unsigned long, personality)
{
	return do_ppc64_personality(personality);
}

#ifdef CONFIG_COMPAT
COMPAT_SYSCALL_DEFINE1(ppc64_personality, unsigned long, personality)
{
	return do_ppc64_personality(personality);
}
#endif /* CONFIG_COMPAT */
#endif /* CONFIG_PPC64 */

SYSCALL_DEFINE6(ppc_fadvise64_64,
		int, fd, int, advice, u32, offset_high, u32, offset_low,
		u32, len_high, u32, len_low)
{
	return ksys_fadvise64_64(fd, merge_64(offset_high, offset_low),
				 merge_64(len_high, len_low), advice);
}

SYSCALL_DEFINE0(switch_endian)
{
	struct thread_info *ti;

	regs_set_return_msr(current->thread.regs,
				current->thread.regs->msr ^ MSR_LE);

	/*
	 * Set TIF_RESTOREALL so that r3 isn't clobbered on return to
	 * userspace. That also has the effect of restoring the non-volatile
	 * GPRs, so we saved them on the way in here.
	 */
	ti = current_thread_info();
	ti->flags |= _TIF_RESTOREALL;

	return 0;
}
