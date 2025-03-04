/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copied from arch/arm64/include/asm/kprobes.h
 *
 * Copyright (C) 2013 Linaro Limited
 * Copyright (C) 2017 SiFive
 */

#ifndef _ASM_RISCV_KPROBES_H
#define _ASM_RISCV_KPROBES_H

#include <asm-generic/kprobes.h>

#ifdef CONFIG_KPROBES
#include <freax/types.h>
#include <freax/ptrace.h>
#include <freax/percpu.h>

#define __ARCH_WANT_KPROBES_INSN_SLOT
#define MAX_INSN_SIZE			2

#define flush_insn_slot(p)		do { } while (0)
#define kretprobe_blacklist_size	0

#include <asm/probes.h>

struct prev_kprobe {
	struct kprobe *kp;
	unsigned int status;
};

/* per-cpu kprobe control block */
struct kprobe_ctlblk {
	unsigned int kprobe_status;
	unsigned long saved_status;
	struct prev_kprobe prev_kprobe;
};

void arch_remove_kprobe(struct kprobe *p);
int kprobe_fault_handler(struct pt_regs *regs, unsigned int trapnr);
bool kprobe_breakpoint_handler(struct pt_regs *regs);
bool kprobe_single_step_handler(struct pt_regs *regs);
#else
static inline bool kprobe_breakpoint_handler(struct pt_regs *regs)
{
	return false;
}

static inline bool kprobe_single_step_handler(struct pt_regs *regs)
{
	return false;
}
#endif /* CONFIG_KPROBES */
#endif /* _ASM_RISCV_KPROBES_H */
