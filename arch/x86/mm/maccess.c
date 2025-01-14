// SPDX-License-Identifier: GPL-2.0-only

#include <freax/uaccess.h>
#include <freax/kernel.h>

#ifdef CONFIG_X86_64
bool copy_from_kernel_nofault_allowed(const void *unsafe_src, size_t size)
{
	unsigned long vaddr = (unsigned long)unsafe_src;

	/*
	 * Do not allow userspace addresses.  This disallows
	 * normal userspace and the userspace guard page:
	 */
	if (vaddr < TASK_SIZE_MAX + PAGE_SIZE)
		return false;

	/*
	 * Allow everything during early boot before 'x86_virt_bits'
	 * is initialized.  Needed for instruction decoding in early
	 * exception handlers.
	 */
	if (!boot_cpu_data.x86_virt_bits)
		return true;

	return __is_canonical_address(vaddr, boot_cpu_data.x86_virt_bits);
}
#else
bool copy_from_kernel_nofault_allowed(const void *unsafe_src, size_t size)
{
	return (unsigned long)unsafe_src >= TASK_SIZE_MAX;
}
#endif
