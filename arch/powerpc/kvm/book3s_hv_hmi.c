// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Hypervisor Maintenance Interrupt (HMI) handling.
 *
 * Copyright 2015 IBM Corporation
 * Author: Mahesh Salgaonkar <mahesh@freax.vnet.ibm.com>
 */

#undef DEBUG

#include <freax/types.h>
#include <freax/compiler.h>
#include <asm/paca.h>
#include <asm/hmi.h>
#include <asm/processor.h>

void wait_for_subcore_guest_exit(void)
{
	int i;

	/*
	 * NULL bitmap pointer indicates that KVM module hasn't
	 * been loaded yet and hence no guests are running, or running
	 * on POWER9 or newer CPU.
	 *
	 * If no KVM is in use, no need to co-ordinate among threads
	 * as all of them will always be in host and no one is going
	 * to modify TB other than the opal hmi handler.
	 *
	 * POWER9 and newer don't need this synchronisation.
	 *
	 * Hence, just return from here.
	 */
	if (!local_paca->sibling_subcore_state)
		return;

	for (i = 0; i < MAX_SUBCORE_PER_CORE; i++)
		while (local_paca->sibling_subcore_state->in_guest[i])
			cpu_relax();
}

void wait_for_tb_resync(void)
{
	if (!local_paca->sibling_subcore_state)
		return;

	while (test_bit(CORE_TB_RESYNC_REQ_BIT,
				&local_paca->sibling_subcore_state->flags))
		cpu_relax();
}
