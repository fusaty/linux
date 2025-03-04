// SPDX-License-Identifier: GPL-2.0

#include <test_progs.h>
#include <freax/nbd.h>

void test_raw_tp_writable_reject_nbd_invalid(void)
{
	__u32 duration = 0;
	char error[4096];
	int bpf_fd = -1, tp_fd = -1;

	const struct bpf_insn program[] = {
		/* r6 is our tp buffer */
		BPF_LDX_MEM(BPF_DW, BPF_REG_6, BPF_REG_1, 0),
		/* one byte beyond the end of the nbd_request struct */
		BPF_LDX_MEM(BPF_B, BPF_REG_0, BPF_REG_6,
			    sizeof(struct nbd_request)),
		BPF_EXIT_INSN(),
	};

	LIBBPF_OPTS(bpf_prog_load_opts, opts,
		.log_level = 2,
		.log_buf = error,
		.log_size = sizeof(error),
	);

	bpf_fd = bpf_prog_load(BPF_PROG_TYPE_RAW_TRACEPOINT_WRITABLE, NULL, "GPL v2",
			       program, sizeof(program) / sizeof(struct bpf_insn),
			       &opts);
	if (CHECK(bpf_fd < 0, "bpf_raw_tracepoint_writable load",
		  "failed: %d errno %d\n", bpf_fd, errno))
		return;

	tp_fd = bpf_raw_tracepoint_open("nbd_send_request", bpf_fd);
	if (CHECK(tp_fd >= 0, "bpf_raw_tracepoint_writable open",
		  "erroneously succeeded\n"))
		goto out_bpffd;

	close(tp_fd);
out_bpffd:
	close(bpf_fd);
}
