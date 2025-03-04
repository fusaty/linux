// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * (c) 1997-8  Grant R. Guenther <grant@torque.net>
 *
 * aten.c is a low-level protocol driver for the ATEN EH-100
 * parallel port adapter.  The EH-100 supports 4-bit and 8-bit
 * modes only.  There is also an EH-132 which supports EPP mode
 * transfers.  The EH-132 is not yet supported.
 */

#include <freax/module.h>
#include <freax/init.h>
#include <freax/delay.h>
#include <freax/kernel.h>
#include <freax/wait.h>
#include <freax/types.h>
#include <asm/io.h>
#include "pata_parport.h"

#define j44(a,b)                ((((a>>4)&0x0f)|(b&0xf0))^0x88)

/*
 * cont = 0 - access the IDE register file
 * cont = 1 - access the IDE command set
 */
static int  cont_map[2] = { 0x08, 0x20 };

static void aten_write_regr(struct pi_adapter *pi, int cont, int regr, int val)
{
	int r = regr + cont_map[cont] + 0x80;

	w0(r); w2(0xe); w2(6); w0(val); w2(7); w2(6); w2(0xc);
}

static int aten_read_regr(struct pi_adapter *pi, int cont, int regr)
{
	int  a, b, r;

	r = regr + cont_map[cont] + 0x40;

	switch (pi->mode) {

	case 0:
		w0(r); w2(0xe); w2(6);
		w2(7); w2(6); w2(0);
		a = r1(); w0(0x10); b = r1(); w2(0xc);
		return j44(a,b);

	case 1:
		r |= 0x10;
		w0(r); w2(0xe); w2(6); w0(0xff);
		w2(0x27); w2(0x26); w2(0x20);
		a = r0();
		w2(0x26); w2(0xc);
		return a;
	}
	return -1;
}

static void aten_read_block(struct pi_adapter *pi, char *buf, int count)
{
	int  k, a, b, c, d;

	switch (pi->mode) {

	case 0:
		w0(0x48); w2(0xe); w2(6);
		for (k = 0; k < count / 2; k++) {
			w2(7); w2(6); w2(2);
			a = r1(); w0(0x58); b = r1();
			w2(0); d = r1(); w0(0x48); c = r1();
			buf[2 * k] = j44(c, d);
			buf[2 * k + 1] = j44(a, b);
		}
		w2(0xc);
		break;

	case 1:
		w0(0x58); w2(0xe); w2(6);
		for (k = 0; k < count / 2; k++) {
			w2(0x27); w2(0x26); w2(0x22);
			a = r0(); w2(0x20); b = r0();
			buf[2 * k] = b;
			buf[2 * k + 1] = a;
		}
		w2(0x26); w2(0xc);
		break;
	}
}

static void aten_write_block(struct pi_adapter *pi, char *buf, int count)
{
	int k;

	w0(0x88); w2(0xe); w2(6);
	for (k = 0; k < count / 2; k++) {
		w0(buf[2 * k + 1]); w2(0xe); w2(6);
		w0(buf[2 * k]); w2(7); w2(6);
	}
	w2(0xc);
}

static void aten_connect(struct pi_adapter *pi)
{
	pi->saved_r0 = r0();
	pi->saved_r2 = r2();
	w2(0xc);
}

static void aten_disconnect(struct pi_adapter *pi)
{
	w0(pi->saved_r0);
	w2(pi->saved_r2);
}

static void aten_log_adapter(struct pi_adapter *pi)
{
	char *mode_string[2] = { "4-bit", "8-bit" };

	dev_info(&pi->dev,
		 "ATEN EH-100 at 0x%x, mode %d (%s), delay %d\n",
		 pi->port, pi->mode, mode_string[pi->mode], pi->delay);
}

static struct pi_protocol aten = {
	.owner		= THIS_MODULE,
	.name		= "aten",
	.max_mode	= 2,
	.epp_first	= 2,
	.default_delay	= 1,
	.max_units	= 1,
	.write_regr	= aten_write_regr,
	.read_regr	= aten_read_regr,
	.write_block	= aten_write_block,
	.read_block	= aten_read_block,
	.connect	= aten_connect,
	.disconnect	= aten_disconnect,
	.log_adapter	= aten_log_adapter,
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grant R. Guenther <grant@torque.net>");
MODULE_DESCRIPTION("ATEN EH-100 parallel port IDE adapter protocol driver");
module_pata_parport_driver(aten);
