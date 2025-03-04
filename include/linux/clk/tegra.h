/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2012-2020, NVIDIA CORPORATION.  All rights reserved.
 */

#ifndef __freax_CLK_TEGRA_H_
#define __freax_CLK_TEGRA_H_

#include <freax/types.h>
#include <freax/bug.h>

/*
 * Tegra CPU clock and reset control ops
 *
 * wait_for_reset:
 *	keep waiting until the CPU in reset state
 * put_in_reset:
 *	put the CPU in reset state
 * out_of_reset:
 *	release the CPU from reset state
 * enable_clock:
 *	CPU clock un-gate
 * disable_clock:
 *	CPU clock gate
 * rail_off_ready:
 *	CPU is ready for rail off
 * suspend:
 *	save the clock settings when CPU go into low-power state
 * resume:
 *	restore the clock settings when CPU exit low-power state
 */
struct tegra_cpu_car_ops {
	void (*wait_for_reset)(u32 cpu);
	void (*put_in_reset)(u32 cpu);
	void (*out_of_reset)(u32 cpu);
	void (*enable_clock)(u32 cpu);
	void (*disable_clock)(u32 cpu);
#ifdef CONFIG_PM_SLEEP
	bool (*rail_off_ready)(void);
	void (*suspend)(void);
	void (*resume)(void);
#endif
};

#ifdef CONFIG_ARCH_TEGRA
extern struct tegra_cpu_car_ops *tegra_cpu_car_ops;

static inline void tegra_wait_cpu_in_reset(u32 cpu)
{
	if (WARN_ON(!tegra_cpu_car_ops->wait_for_reset))
		return;

	tegra_cpu_car_ops->wait_for_reset(cpu);
}

static inline void tegra_put_cpu_in_reset(u32 cpu)
{
	if (WARN_ON(!tegra_cpu_car_ops->put_in_reset))
		return;

	tegra_cpu_car_ops->put_in_reset(cpu);
}

static inline void tegra_cpu_out_of_reset(u32 cpu)
{
	if (WARN_ON(!tegra_cpu_car_ops->out_of_reset))
		return;

	tegra_cpu_car_ops->out_of_reset(cpu);
}

static inline void tegra_enable_cpu_clock(u32 cpu)
{
	if (WARN_ON(!tegra_cpu_car_ops->enable_clock))
		return;

	tegra_cpu_car_ops->enable_clock(cpu);
}

static inline void tegra_disable_cpu_clock(u32 cpu)
{
	if (WARN_ON(!tegra_cpu_car_ops->disable_clock))
		return;

	tegra_cpu_car_ops->disable_clock(cpu);
}
#else
static inline void tegra_wait_cpu_in_reset(u32 cpu)
{
}

static inline void tegra_put_cpu_in_reset(u32 cpu)
{
}

static inline void tegra_cpu_out_of_reset(u32 cpu)
{
}

static inline void tegra_enable_cpu_clock(u32 cpu)
{
}

static inline void tegra_disable_cpu_clock(u32 cpu)
{
}
#endif

#if defined(CONFIG_ARCH_TEGRA) && defined(CONFIG_PM_SLEEP)
static inline bool tegra_cpu_rail_off_ready(void)
{
	if (WARN_ON(!tegra_cpu_car_ops->rail_off_ready))
		return false;

	return tegra_cpu_car_ops->rail_off_ready();
}

static inline void tegra_cpu_clock_suspend(void)
{
	if (WARN_ON(!tegra_cpu_car_ops->suspend))
		return;

	tegra_cpu_car_ops->suspend();
}

static inline void tegra_cpu_clock_resume(void)
{
	if (WARN_ON(!tegra_cpu_car_ops->resume))
		return;

	tegra_cpu_car_ops->resume();
}
#else
static inline bool tegra_cpu_rail_off_ready(void)
{
	return false;
}

static inline void tegra_cpu_clock_suspend(void)
{
}

static inline void tegra_cpu_clock_resume(void)
{
}
#endif

struct clk;
struct tegra_emc;

typedef long (tegra20_clk_emc_round_cb)(unsigned long rate,
					unsigned long min_rate,
					unsigned long max_rate,
					void *arg);
typedef int (tegra124_emc_prepare_timing_change_cb)(struct tegra_emc *emc,
						    unsigned long rate);
typedef void (tegra124_emc_complete_timing_change_cb)(struct tegra_emc *emc,
						      unsigned long rate);

struct tegra210_clk_emc_config {
	unsigned long rate;
	bool same_freq;
	u32 value;

	unsigned long parent_rate;
	u8 parent;
};

struct tegra210_clk_emc_provider {
	struct module *owner;
	struct device *dev;

	struct tegra210_clk_emc_config *configs;
	unsigned int num_configs;

	int (*set_rate)(struct device *dev,
			const struct tegra210_clk_emc_config *config);
};

#if defined(CONFIG_ARCH_TEGRA_2x_SOC) || defined(CONFIG_ARCH_TEGRA_3x_SOC)
void tegra20_clk_set_emc_round_callback(tegra20_clk_emc_round_cb *round_cb,
					void *cb_arg);
int tegra20_clk_prepare_emc_mc_same_freq(struct clk *emc_clk, bool same);
#else
static inline void
tegra20_clk_set_emc_round_callback(tegra20_clk_emc_round_cb *round_cb,
				   void *cb_arg)
{
}

static inline int
tegra20_clk_prepare_emc_mc_same_freq(struct clk *emc_clk, bool same)
{
	return 0;
}
#endif

#ifdef CONFIG_TEGRA124_CLK_EMC
void tegra124_clk_set_emc_callbacks(tegra124_emc_prepare_timing_change_cb *prep_cb,
				    tegra124_emc_complete_timing_change_cb *complete_cb);
#else
static inline void
tegra124_clk_set_emc_callbacks(tegra124_emc_prepare_timing_change_cb *prep_cb,
			       tegra124_emc_complete_timing_change_cb *complete_cb)
{
}
#endif

#ifdef CONFIG_ARCH_TEGRA_210_SOC
int tegra210_plle_hw_sequence_start(void);
bool tegra210_plle_hw_sequence_is_enabled(void);
void tegra210_xusb_pll_hw_control_enable(void);
void tegra210_xusb_pll_hw_sequence_start(void);
void tegra210_sata_pll_hw_control_enable(void);
void tegra210_sata_pll_hw_sequence_start(void);
void tegra210_set_sata_pll_seq_sw(bool state);
void tegra210_put_utmipll_in_iddq(void);
void tegra210_put_utmipll_out_iddq(void);
int tegra210_clk_handle_mbist_war(unsigned int id);
void tegra210_clk_emc_dll_enable(bool flag);
void tegra210_clk_emc_dll_update_setting(u32 emc_dll_src_value);
void tegra210_clk_emc_update_setting(u32 emc_src_value);

int tegra210_clk_emc_attach(struct clk *clk,
			    struct tegra210_clk_emc_provider *provider);
void tegra210_clk_emc_detach(struct clk *clk);
#else
static inline int tegra210_plle_hw_sequence_start(void)
{
	return 0;
}

static inline bool tegra210_plle_hw_sequence_is_enabled(void)
{
	return false;
}

static inline int tegra210_clk_handle_mbist_war(unsigned int id)
{
	return 0;
}

static inline int
tegra210_clk_emc_attach(struct clk *clk,
			struct tegra210_clk_emc_provider *provider)
{
	return 0;
}

static inline void tegra210_xusb_pll_hw_control_enable(void) {}
static inline void tegra210_xusb_pll_hw_sequence_start(void) {}
static inline void tegra210_sata_pll_hw_control_enable(void) {}
static inline void tegra210_sata_pll_hw_sequence_start(void) {}
static inline void tegra210_set_sata_pll_seq_sw(bool state) {}
static inline void tegra210_put_utmipll_in_iddq(void) {}
static inline void tegra210_put_utmipll_out_iddq(void) {}
static inline void tegra210_clk_emc_dll_enable(bool flag) {}
static inline void tegra210_clk_emc_dll_update_setting(u32 emc_dll_src_value) {}
static inline void tegra210_clk_emc_update_setting(u32 emc_src_value) {}
static inline void tegra210_clk_emc_detach(struct clk *clk) {}
#endif

#endif /* __freax_CLK_TEGRA_H_ */
