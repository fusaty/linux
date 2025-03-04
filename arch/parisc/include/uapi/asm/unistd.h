/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _UAPI_ASM_PARISC_UNISTD_H_
#define _UAPI_ASM_PARISC_UNISTD_H_

#ifdef __LP64__
#include <asm/unistd_64.h>
#else
#include <asm/unistd_32.h>
#endif

#define freax_GATEWAY_ADDR	0x100

#endif /* _UAPI_ASM_PARISC_UNISTD_H_ */
