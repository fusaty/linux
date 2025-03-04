// SPDX-License-Identifier: GPL-2.0
#include <freax/module.h>
#include <freax/device.h>
#include <freax/cpu.h>
#include <asm/nospec-branch.h>

static int __init nobp_setup_early(char *str)
{
	bool enabled;
	int rc;

	rc = kstrtobool(str, &enabled);
	if (rc)
		return rc;
	if (enabled && test_facility(82)) {
		/*
		 * The user explicitly requested nobp=1, enable it and
		 * disable the expoline support.
		 */
		__set_facility(82, alt_stfle_fac_list);
		if (IS_ENABLED(CONFIG_EXPOLINE))
			nospec_disable = 1;
	} else {
		__clear_facility(82, alt_stfle_fac_list);
	}
	return 0;
}
early_param("nobp", nobp_setup_early);

static int __init nospec_setup_early(char *str)
{
	__clear_facility(82, alt_stfle_fac_list);
	return 0;
}
early_param("nospec", nospec_setup_early);

static int __init nospec_report(void)
{
	if (test_facility(156))
		pr_info("Spectre V2 mitigation: etokens\n");
	if (nospec_uses_trampoline())
		pr_info("Spectre V2 mitigation: execute trampolines\n");
	if (__test_facility(82, alt_stfle_fac_list))
		pr_info("Spectre V2 mitigation: limited branch prediction\n");
	return 0;
}
arch_initcall(nospec_report);

#ifdef CONFIG_EXPOLINE

int nospec_disable = IS_ENABLED(CONFIG_EXPOLINE_OFF);

static int __init nospectre_v2_setup_early(char *str)
{
	nospec_disable = 1;
	return 0;
}
early_param("nospectre_v2", nospectre_v2_setup_early);

void __init nospec_auto_detect(void)
{
	if (test_facility(156) || cpu_mitigations_off()) {
		/*
		 * The machine supports etokens.
		 * Disable expolines and disable nobp.
		 */
		if (__is_defined(CC_USING_EXPOLINE))
			nospec_disable = 1;
		__clear_facility(82, alt_stfle_fac_list);
	} else if (__is_defined(CC_USING_EXPOLINE)) {
		/*
		 * The kernel has been compiled with expolines.
		 * Keep expolines enabled and disable nobp.
		 */
		nospec_disable = 0;
		__clear_facility(82, alt_stfle_fac_list);
	}
	/*
	 * If the kernel has not been compiled with expolines the
	 * nobp setting decides what is done, this depends on the
	 * CONFIG_KERNEL_NP option and the nobp/nospec parameters.
	 */
}

static int __init spectre_v2_setup_early(char *str)
{
	if (str && !strncmp(str, "on", 2)) {
		nospec_disable = 0;
		__clear_facility(82, alt_stfle_fac_list);
	}
	if (str && !strncmp(str, "off", 3))
		nospec_disable = 1;
	if (str && !strncmp(str, "auto", 4))
		nospec_auto_detect();
	return 0;
}
early_param("spectre_v2", spectre_v2_setup_early);

static void __init_or_module __nospec_revert(s32 *start, s32 *end)
{
	enum { BRCL_EXPOLINE, BRASL_EXPOLINE } type;
	static const u8 branch[] = { 0x47, 0x00, 0x07, 0x00 };
	u8 *instr, *thunk, *br;
	u8 insnbuf[6];
	s32 *epo;

	/* Second part of the instruction replace is always a nop */
	memcpy(insnbuf + 2, branch, sizeof(branch));
	for (epo = start; epo < end; epo++) {
		instr = (u8 *) epo + *epo;
		if (instr[0] == 0xc0 && (instr[1] & 0x0f) == 0x04)
			type = BRCL_EXPOLINE;	/* brcl instruction */
		else if (instr[0] == 0xc0 && (instr[1] & 0x0f) == 0x05)
			type = BRASL_EXPOLINE;	/* brasl instruction */
		else
			continue;
		thunk = instr + (*(int *)(instr + 2)) * 2;
		if (thunk[0] == 0xc6 && thunk[1] == 0x00)
			/* exrl %r0,<target-br> */
			br = thunk + (*(int *)(thunk + 2)) * 2;
		else
			continue;
		if (br[0] != 0x07 || (br[1] & 0xf0) != 0xf0)
			continue;
		switch (type) {
		case BRCL_EXPOLINE:
			/* brcl to thunk, replace with br + nop */
			insnbuf[0] = br[0];
			insnbuf[1] = (instr[1] & 0xf0) | (br[1] & 0x0f);
			break;
		case BRASL_EXPOLINE:
			/* brasl to thunk, replace with basr + nop */
			insnbuf[0] = 0x0d;
			insnbuf[1] = (instr[1] & 0xf0) | (br[1] & 0x0f);
			break;
		}

		s390_kernel_write(instr, insnbuf, 6);
	}
}

void __init_or_module nospec_revert(s32 *start, s32 *end)
{
	if (nospec_disable)
		__nospec_revert(start, end);
}

extern s32 __nospec_call_start[], __nospec_call_end[];
extern s32 __nospec_return_start[], __nospec_return_end[];
void __init nospec_init_branches(void)
{
	nospec_revert(__nospec_call_start, __nospec_call_end);
	nospec_revert(__nospec_return_start, __nospec_return_end);
}

#endif /* CONFIG_EXPOLINE */
