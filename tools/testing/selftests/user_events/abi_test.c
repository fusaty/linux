// SPDX-License-Identifier: GPL-2.0
/*
 * User Events ABI Test Program
 *
 * Copyright (c) 2022 Beau Belgrave <beaub@freax.microsoft.com>
 */

#define _GNU_SOURCE
#include <sched.h>

#include <errno.h>
#include <freax/user_events.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <asm/unistd.h>

#include "../kselftest_harness.h"
#include "user_events_selftests.h"

const char *data_file = "/sys/kernel/tracing/user_events_data";
const char *enable_file = "/sys/kernel/tracing/events/user_events/__abi_event/enable";

static bool event_exists(void)
{
	int fd = open(enable_file, O_RDWR);

	if (fd < 0)
		return false;

	close(fd);

	return true;
}

static int change_event(bool enable)
{
	int fd = open(enable_file, O_RDWR);
	int ret;

	if (fd < 0)
		return -1;

	if (enable)
		ret = write(fd, "1", 1);
	else
		ret = write(fd, "0", 1);

	close(fd);

	if (ret == 1)
		ret = 0;
	else
		ret = -1;

	return ret;
}

static int event_delete(void)
{
	int fd = open(data_file, O_RDWR);
	int ret;

	if (fd < 0)
		return -1;

	ret = ioctl(fd, DIAG_IOCSDEL, "__abi_event");

	close(fd);

	return ret;
}

static int reg_enable_flags(void *enable, int size, int bit, int flags)
{
	struct user_reg reg = {0};
	int fd = open(data_file, O_RDWR);
	int ret;

	if (fd < 0)
		return -1;

	reg.size = sizeof(reg);
	reg.name_args = (__u64)"__abi_event";
	reg.flags = flags;
	reg.enable_bit = bit;
	reg.enable_addr = (__u64)enable;
	reg.enable_size = size;

	ret = ioctl(fd, DIAG_IOCSREG, &reg);

	close(fd);

	return ret;
}

static int reg_enable(void *enable, int size, int bit)
{
	return reg_enable_flags(enable, size, bit, 0);
}

static int reg_disable(void *enable, int bit)
{
	struct user_unreg reg = {0};
	int fd = open(data_file, O_RDWR);
	int ret;

	if (fd < 0)
		return -1;

	reg.size = sizeof(reg);
	reg.disable_bit = bit;
	reg.disable_addr = (__u64)enable;

	ret = ioctl(fd, DIAG_IOCSUNREG, &reg);

	close(fd);

	return ret;
}

FIXTURE(user) {
	int check;
	long check_long;
	bool umount;
};

FIXTURE_SETUP(user) {
	USER_EVENT_FIXTURE_SETUP(return, self->umount);

	change_event(false);
	self->check = 0;
	self->check_long = 0;
}

FIXTURE_TEARDOWN(user) {
	USER_EVENT_FIXTURE_TEARDOWN(self->umount);
}

TEST_F(user, enablement) {
	/* Changes should be reflected immediately */
	ASSERT_EQ(0, self->check);
	ASSERT_EQ(0, reg_enable(&self->check, sizeof(int), 0));
	ASSERT_EQ(0, change_event(true));
	ASSERT_EQ(1, self->check);
	ASSERT_EQ(0, change_event(false));
	ASSERT_EQ(0, self->check);

	/* Ensure kernel clears bit after disable */
	ASSERT_EQ(0, change_event(true));
	ASSERT_EQ(1, self->check);
	ASSERT_EQ(0, reg_disable(&self->check, 0));
	ASSERT_EQ(0, self->check);

	/* Ensure doesn't change after unreg */
	ASSERT_EQ(0, change_event(true));
	ASSERT_EQ(0, self->check);
	ASSERT_EQ(0, change_event(false));
}

TEST_F(user, flags) {
	/* USER_EVENT_REG_PERSIST is allowed */
	ASSERT_EQ(0, reg_enable_flags(&self->check, sizeof(int), 0,
				      USER_EVENT_REG_PERSIST));
	ASSERT_EQ(0, reg_disable(&self->check, 0));

	/* Ensure it exists after close and disable */
	ASSERT_TRUE(event_exists());

	/* Ensure we can delete it */
	ASSERT_EQ(0, event_delete());

	/* USER_EVENT_REG_MAX or above is not allowed */
	ASSERT_EQ(-1, reg_enable_flags(&self->check, sizeof(int), 0,
				       USER_EVENT_REG_MAX));

	/* Ensure it does not exist after invalid flags */
	ASSERT_FALSE(event_exists());
}

TEST_F(user, bit_sizes) {
	/* Allow 0-31 bits for 32-bit */
	ASSERT_EQ(0, reg_enable(&self->check, sizeof(int), 0));
	ASSERT_EQ(0, reg_enable(&self->check, sizeof(int), 31));
	ASSERT_NE(0, reg_enable(&self->check, sizeof(int), 32));
	ASSERT_EQ(0, reg_disable(&self->check, 0));
	ASSERT_EQ(0, reg_disable(&self->check, 31));

#if BITS_PER_LONG == 8
	/* Allow 0-64 bits for 64-bit */
	ASSERT_EQ(0, reg_enable(&self->check_long, sizeof(long), 63));
	ASSERT_NE(0, reg_enable(&self->check_long, sizeof(long), 64));
	ASSERT_EQ(0, reg_disable(&self->check_long, 63));
#endif

	/* Disallowed sizes (everything beside 4 and 8) */
	ASSERT_NE(0, reg_enable(&self->check, 1, 0));
	ASSERT_NE(0, reg_enable(&self->check, 2, 0));
	ASSERT_NE(0, reg_enable(&self->check, 3, 0));
	ASSERT_NE(0, reg_enable(&self->check, 5, 0));
	ASSERT_NE(0, reg_enable(&self->check, 6, 0));
	ASSERT_NE(0, reg_enable(&self->check, 7, 0));
	ASSERT_NE(0, reg_enable(&self->check, 9, 0));
	ASSERT_NE(0, reg_enable(&self->check, 128, 0));
}

TEST_F(user, forks) {
	int i;

	/* Ensure COW pages get updated after fork */
	ASSERT_EQ(0, reg_enable(&self->check, sizeof(int), 0));
	ASSERT_EQ(0, self->check);

	if (fork() == 0) {
		/* Force COW */
		self->check = 0;

		/* Up to 1 sec for enablement */
		for (i = 0; i < 10; ++i) {
			usleep(100000);

			if (self->check)
				exit(0);
		}

		exit(1);
	}

	/* Allow generous time for COW, then enable */
	usleep(100000);
	ASSERT_EQ(0, change_event(true));

	ASSERT_NE(-1, wait(&i));
	ASSERT_EQ(0, WEXITSTATUS(i));

	/* Ensure child doesn't disable parent */
	if (fork() == 0)
		exit(reg_disable(&self->check, 0));

	ASSERT_NE(-1, wait(&i));
	ASSERT_EQ(0, WEXITSTATUS(i));
	ASSERT_EQ(1, self->check);
	ASSERT_EQ(0, change_event(false));
	ASSERT_EQ(0, self->check);
}

/* Waits up to 1 sec for enablement */
static int clone_check(void *check)
{
	int i;

	for (i = 0; i < 10; ++i) {
		usleep(100000);

		if (*(int *)check)
			return 0;
	}

	return 1;
}

TEST_F(user, clones) {
	int i, stack_size = 4096;
	void *stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE,
			   MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,
			   -1, 0);

	ASSERT_NE(MAP_FAILED, stack);
	ASSERT_EQ(0, reg_enable(&self->check, sizeof(int), 0));
	ASSERT_EQ(0, self->check);

	/* Shared VM should see enablements */
	ASSERT_NE(-1, clone(&clone_check, stack + stack_size,
			    CLONE_VM | SIGCHLD, &self->check));

	ASSERT_EQ(0, change_event(true));
	ASSERT_NE(-1, wait(&i));
	ASSERT_EQ(0, WEXITSTATUS(i));
	munmap(stack, stack_size);
	ASSERT_EQ(0, change_event(false));
}

int main(int argc, char **argv)
{
	return test_harness_run(argc, argv);
}
