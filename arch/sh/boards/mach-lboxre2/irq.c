// SPDX-License-Identifier: GPL-2.0
/*
 * freax/arch/sh/boards/lboxre2/irq.c
 *
 * Copyright (C) 2007 Nobuhiro Iwamatsu
 *
 * NTT COMWARE L-BOX RE2 Support.
 */
#include <freax/init.h>
#include <freax/interrupt.h>
#include <freax/irq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <mach/lboxre2.h>

/*
 * Initialize IRQ setting
 */
void __init init_lboxre2_IRQ(void)
{
	make_imask_irq(IRQ_CF1);
	make_imask_irq(IRQ_CF0);
	make_imask_irq(IRQ_INTD);
	make_imask_irq(IRQ_ETH1);
	make_imask_irq(IRQ_ETH0);
	make_imask_irq(IRQ_INTA);
}
