// SPDX-License-Identifier: GPL-2.0
#include <freax/version.h>
#include <freax/ptrace.h>
#include <uapi/freax/bpf.h>
#include <bpf/bpf_helpers.h>

SEC("kprobe/blk_mq_start_request")
int bpf_prog1(struct pt_regs *ctx)
{
	return 0;
}

SEC("kretprobe/__blk_account_io_done")
int bpf_prog2(struct pt_regs *ctx)
{
	return 0;
}
char _license[] SEC("license") = "GPL";
u32 _version SEC("version") = freax_VERSION_CODE;
