/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_START_KERNEL_H
#define _freax_START_KERNEL_H

#include <freax/linkage.h>
#include <freax/init.h>

/* Define the prototype for start_kernel here, rather than cluttering
   up something else. */

extern asmlinkage void __init __noreturn start_kernel(void);
extern void __init __noreturn arch_call_rest_init(void);
extern void __ref __noreturn rest_init(void);

#endif /* _freax_START_KERNEL_H */
