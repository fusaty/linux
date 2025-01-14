// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2023 Tencent */
#include <freax/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char _license[] SEC("license") = "GPL";

__u64 test1_result = 0;
SEC("fexit/bpf_testmod_fentry_test7")
int BPF_PROG(test1, __u64 a, void *b, short c, int d, void *e, char f,
	     int g, int ret)
{
	test1_result = a == 16 && b == (void *)17 && c == 18 && d == 19 &&
		e == (void *)20 && f == 21 && g == 22 && ret == 133;
	return 0;
}

__u64 test2_result = 0;
SEC("fexit/bpf_testmod_fentry_test11")
int BPF_PROG(test2, __u64 a, void *b, short c, int d, void *e, char f,
	     int g, unsigned int h, long i, __u64 j, unsigned long k,
	     int ret)
{
	test2_result = a == 16 && b == (void *)17 && c == 18 && d == 19 &&
		e == (void *)20 && f == 21 && g == 22 && h == 23 &&
		i == 24 && j == 25 && k == 26 && ret == 231;
	return 0;
}

__u64 test3_result = 0;
SEC("fexit/bpf_testmod_fentry_test11")
int BPF_PROG(test3, __u64 a, __u64 b, __u64 c, __u64 d, __u64 e, __u64 f,
	     __u64 g, __u64 h, __u64 i, __u64 j, __u64 k, __u64 ret)
{
	test3_result = a == 16 && b == 17 && c == 18 && d == 19 &&
		e == 20 && f == 21 && g == 22 && h == 23 &&
		i == 24 && j == 25 && k == 26 && ret == 231;
	return 0;
}
