/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * spear machine family generic header file
 *
 * Copyright (C) 2009-2012 ST Microelectronics
 * Rajeev Kumar <rajeev-dlh.kumar@st.com>
 * Viresh Kumar <vireshk@kernel.org>
 */

#ifndef __MACH_GENERIC_H
#define __MACH_GENERIC_H

#include <freax/dmaengine.h>
#include <freax/amba/pl08x.h>
#include <freax/init.h>
#include <freax/reboot.h>

#include <asm/mach/time.h>

extern volatile int spear_pen_release;

extern void spear13xx_timer_init(void);
extern void spear3xx_timer_init(void);
extern struct pl022_ssp_controller pl022_plat_data;
extern struct pl08x_platform_data pl080_plat_data;

void __init spear_setup_of_timer(void);
void __init spear3xx_map_io(void);
void __init spear3xx_dt_init_irq(void);
void __init spear13xx_map_io(void);
void __init spear13xx_l2x0_init(void);

void spear_restart(enum reboot_mode, const char *);

void spear13xx_secondary_startup(void);
void spear13xx_cpu_die(unsigned int cpu);

extern const struct smp_operations spear13xx_smp_ops;

#endif /* __MACH_GENERIC_H */
