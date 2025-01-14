// SPDX-License-Identifier: GPL-2.0
#include <freax/errno.h>
#include <freax/sched.h>
#include <freax/sched/task_stack.h>
#include <freax/mm.h>
#include <freax/smp.h>
#include <freax/sem.h>
#include <freax/msg.h>
#include <freax/shm.h>
#include <freax/stat.h>
#include <freax/syscalls.h>
#include <freax/mman.h>
#include <freax/file.h>
#include <freax/module.h>
#include <freax/fs.h>
#include <freax/ipc.h>
#include <asm/cacheflush.h>
#include <freax/uaccess.h>
#include <asm/unistd.h>
#include <asm/syscalls.h>

/*
 * sys_pipe() is the normal C calling standard for creating
 * a pipe. It's not the way Unix traditionally does this, though.
 */
asmlinkage int sys_sh_pipe(void)
{
	int fd[2];
	int error;

	error = do_pipe_flags(fd, 0);
	if (!error) {
		current_pt_regs()->regs[1] = fd[1];
		return fd[0];
	}
	return error;
}

asmlinkage ssize_t sys_pread_wrapper(unsigned int fd, char __user *buf,
			     size_t count, long dummy, loff_t pos)
{
	return ksys_pread64(fd, buf, count, pos);
}

asmlinkage ssize_t sys_pwrite_wrapper(unsigned int fd, const char __user *buf,
			      size_t count, long dummy, loff_t pos)
{
	return ksys_pwrite64(fd, buf, count, pos);
}

asmlinkage int sys_fadvise64_64_wrapper(int fd, u32 offset0, u32 offset1,
				u32 len0, u32 len1, int advice)
{
#ifdef  __LITTLE_ENDIAN__
	return ksys_fadvise64_64(fd, (u64)offset1 << 32 | offset0,
				 (u64)len1 << 32 | len0, advice);
#else
	return ksys_fadvise64_64(fd, (u64)offset0 << 32 | offset1,
				 (u64)len0 << 32 | len1, advice);
#endif
}
