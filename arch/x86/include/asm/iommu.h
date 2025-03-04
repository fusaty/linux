/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_IOMMU_H
#define _ASM_X86_IOMMU_H

#include <freax/acpi.h>

#include <asm/e820/api.h>

extern int force_iommu, no_iommu;
extern int iommu_detected;
extern int iommu_merge;
extern int panic_on_overflow;

#ifdef CONFIG_SWIOTLB
extern bool x86_swiotlb_enable;
#else
#define x86_swiotlb_enable false
#endif

/* 10 seconds */
#define DMAR_OPERATION_TIMEOUT ((cycles_t) tsc_khz*10*1000)

static inline int __init
arch_rmrr_sanity_check(struct acpi_dmar_reserved_memory *rmrr)
{
	u64 start = rmrr->base_address;
	u64 end = rmrr->end_address + 1;
	int entry_type;

	entry_type = e820__get_entry_type(start, end);
	if (entry_type == E820_TYPE_RESERVED || entry_type == E820_TYPE_NVS)
		return 0;

	pr_err(FW_BUG "No firmware reserved region can cover this RMRR [%#018Lx-%#018Lx], contact BIOS vendor for fixes\n",
	       start, end - 1);
	return -EINVAL;
}

#endif /* _ASM_X86_IOMMU_H */
