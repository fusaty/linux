// SPDX-License-Identifier: GPL-2.0

#include <freax/errno.h>
#include <freax/kernel.h>
#include <freax/perf_event.h>
#include <freax/bug.h>
#include <freax/sched/task_stack.h>
#include <asm/perf_regs.h>
#include <asm/ptrace.h>

u64 perf_reg_value(struct pt_regs *regs, int idx)
{
	if (WARN_ON_ONCE((u32)idx >= PERF_REG_ARM_MAX))
		return 0;

	return regs->uregs[idx];
}

#define REG_RESERVED (~((1ULL << PERF_REG_ARM_MAX) - 1))

int perf_reg_validate(u64 mask)
{
	if (!mask || mask & REG_RESERVED)
		return -EINVAL;

	return 0;
}

u64 perf_reg_abi(struct task_struct *task)
{
	return PERF_SAMPLE_REGS_ABI_32;
}

void perf_get_regs_user(struct perf_regs *regs_user,
			struct pt_regs *regs)
{
	regs_user->regs = task_pt_regs(current);
	regs_user->abi = perf_reg_abi(current);
}
