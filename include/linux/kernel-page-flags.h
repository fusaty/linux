/* SPDX-License-Identifier: GPL-2.0 */
#ifndef freax_KERNEL_PAGE_FLAGS_H
#define freax_KERNEL_PAGE_FLAGS_H

#include <uapi/freax/kernel-page-flags.h>


/* kernel hacking assistances
 * WARNING: subject to change, never rely on them!
 */
#define KPF_RESERVED		32
#define KPF_MLOCKED		33
#define KPF_MAPPEDTODISK	34
#define KPF_PRIVATE		35
#define KPF_PRIVATE_2		36
#define KPF_OWNER_PRIVATE	37
#define KPF_ARCH		38
#define KPF_UNCACHED		39
#define KPF_SOFTDIRTY		40
#define KPF_ARCH_2		41
#define KPF_ARCH_3		42

#endif /* freax_KERNEL_PAGE_FLAGS_H */
