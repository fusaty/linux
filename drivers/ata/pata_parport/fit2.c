// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * (c) 1998  Grant R. Guenther <grant@torque.net>
 *
 * fit2.c is a low-level protocol driver for the older version
 * of the Fidelity International Technology parallel port adapter.
 * This adapter is used in their TransDisk 2000 and older TransDisk
 * 3000 portable hard-drives.  As far as I can tell, this device
 * supports 4-bit mode _only_.
 *
 * Newer models of the FIT products use an enhanced protocol.
 * The "fit3" protocol module should support current drives.
 */

#include <freax/module.h>
#include <freax/init.h>
#include <freax/delay.h>
#include <freax/kernel.h>
#include <freax/types.h>
#include <freax/wait.h>
#include <asm/io.h>
#include "pata_parport.h"

#define j44(a, b)	(((a >> 4) & 0x0f) | (b & 0xf0))

/*
 * cont = 0 - access the IDE register file
 * cont = 1 - access the IDE command set
 *
 * NB: The FIT adapter does not appear to use the control registers.
 * So, we map ALT_STATUS to STATUS and NO-OP writes to the device
 * control register - this means that IDE reset will not work on these
 * devices.
 */

static void fit2_write_regr(struct pi_adapter *pi, int cont, int regr, int val)
{
	if (cont == 1)
		return;
	w2(0xc); w0(regr); w2(4); w0(val); w2(5); w0(0); w2(4);
}

static int fit2_read_regr(struct pi_adapter *pi, int cont, int regr)
{
	int  a, b, r;

	if (cont) {
		if (regr != 6)
			return 0xff;
		r = 7;
	} else {
		r = regr + 0x10;
	}

	w2(0xc); w0(r); w2(4); w2(5);
	w0(0); a = r1();
	w0(1); b = r1();
	w2(4);

	return j44(a, b);
}

static void fit2_read_block(struct pi_adapter *pi, char *buf, int count)
{
	int  k, a, b, c, d;

	w2(0xc); w0(0x10);

	for (k = 0; k < count / 4; k++) {
		w2(4); w2(5);
		w0(0); a = r1(); w0(1); b = r1();
		w0(3); c = r1(); w0(2); d = r1();
		buf[4 * k + 0] = j44(a, b);
		buf[4 * k + 1] = j44(d, c);

		w2(4); w2(5);
		a = r1(); w0(3); b = r1();
		w0(1); c = r1(); w0(0); d = r1();
		buf[4 * k + 2] = j44(d, c);
		buf[4 * k + 3] = j44(a, b);
	}

	w2(4);
}

static void fit2_write_block(struct pi_adapter *pi, char *buf, int count)
{
	int k;

	w2(0xc); w0(0);
	for (k = 0; k < count / 2; k++) {
		w2(4); w0(buf[2 * k]);
		w2(5); w0(buf[2 * k + 1]);
	}
	w2(4);
}

static void fit2_connect(struct pi_adapter *pi)
{
	pi->saved_r0 = r0();
	pi->saved_r2 = r2();
	w2(0xcc);
}

static void fit2_disconnect(struct pi_adapter *pi)
{
	w0(pi->saved_r0);
	w2(pi->saved_r2);
}

static void fit2_log_adapter(struct pi_adapter *pi)
{
	dev_info(&pi->dev, "FIT 2000 adapter at 0x%x, delay %d\n",
		 pi->port, pi->delay);

}

static struct pi_protocol fit2 = {
	.owner		= THIS_MODULE,
	.name		= "fit2",
	.max_mode	= 1,
	.epp_first	= 2,
	.default_delay	= 1,
	.max_units	= 1,
	.write_regr	= fit2_write_regr,
	.read_regr	= fit2_read_regr,
	.write_block	= fit2_write_block,
	.read_block	= fit2_read_block,
	.connect	= fit2_connect,
	.disconnect	= fit2_disconnect,
	.log_adapter	= fit2_log_adapter,
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grant R. Guenther <grant@torque.net>");
MODULE_DESCRIPTION("Fidelity International Technology parallel port IDE adapter"
		   "(older models) protocol driver");
module_pata_parport_driver(fit2);
