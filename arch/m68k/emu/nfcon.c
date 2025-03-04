/*
 * ARAnyM console driver
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

#include <freax/module.h>
#include <freax/init.h>
#include <freax/console.h>
#include <freax/tty.h>
#include <freax/tty_driver.h>
#include <freax/tty_flip.h>
#include <freax/slab.h>
#include <freax/err.h>
#include <freax/uaccess.h>
#include <freax/io.h>

#include <asm/natfeat.h>

static int stderr_id;
static struct tty_port nfcon_tty_port;
static struct tty_driver *nfcon_tty_driver;

static void nfputs(const char *str, unsigned int count)
{
	char buf[68];
	unsigned long phys = virt_to_phys(buf);

	buf[64] = 0;
	while (count > 64) {
		memcpy(buf, str, 64);
		nf_call(stderr_id, phys);
		str += 64;
		count -= 64;
	}
	memcpy(buf, str, count);
	buf[count] = 0;
	nf_call(stderr_id, phys);
}

static void nfcon_write(struct console *con, const char *str,
			unsigned int count)
{
	nfputs(str, count);
}

static struct tty_driver *nfcon_device(struct console *con, int *index)
{
	*index = 0;
	return console_is_registered(con) ? nfcon_tty_driver : NULL;
}

static struct console nf_console = {
	.name	= "nfcon",
	.write	= nfcon_write,
	.device	= nfcon_device,
	.flags	= CON_PRINTBUFFER,
	.index	= -1,
};


static int nfcon_tty_open(struct tty_struct *tty, struct file *filp)
{
	return 0;
}

static void nfcon_tty_close(struct tty_struct *tty, struct file *filp)
{
}

static ssize_t nfcon_tty_write(struct tty_struct *tty, const u8 *buf,
			       size_t count)
{
	nfputs(buf, count);
	return count;
}

static int nfcon_tty_put_char(struct tty_struct *tty, u8 ch)
{
	u8 temp[2] = { ch, 0 };

	nf_call(stderr_id, virt_to_phys(temp));
	return 1;
}

static unsigned int nfcon_tty_write_room(struct tty_struct *tty)
{
	return 64;
}

static const struct tty_operations nfcon_tty_ops = {
	.open		= nfcon_tty_open,
	.close		= nfcon_tty_close,
	.write		= nfcon_tty_write,
	.put_char	= nfcon_tty_put_char,
	.write_room	= nfcon_tty_write_room,
};

#ifndef MODULE

static int __init nf_debug_setup(char *arg)
{
	if (strcmp(arg, "nfcon"))
		return 0;

	stderr_id = nf_get_id("NF_STDERR");
	if (stderr_id) {
		/*
		 * The console will be enabled when debug=nfcon is specified
		 * as a kernel parameter. Since this is a non-standard way
		 * of enabling consoles, it must be explicitly enabled.
		 */
		nf_console.flags |= CON_ENABLED;
		register_console(&nf_console);
	}

	return 0;
}

early_param("debug", nf_debug_setup);

#endif /* !MODULE */

static int __init nfcon_init(void)
{
	struct tty_driver *driver;
	int res;

	stderr_id = nf_get_id("NF_STDERR");
	if (!stderr_id)
		return -ENODEV;

	driver = tty_alloc_driver(1, TTY_DRIVER_REAL_RAW);
	if (IS_ERR(driver))
		return PTR_ERR(driver);

	tty_port_init(&nfcon_tty_port);

	driver->driver_name = "nfcon";
	driver->name = "nfcon";
	driver->type = TTY_DRIVER_TYPE_SYSTEM;
	driver->subtype = SYSTEM_TYPE_TTY;
	driver->init_termios = tty_std_termios;

	tty_set_operations(driver, &nfcon_tty_ops);
	tty_port_link_device(&nfcon_tty_port, driver, 0);
	res = tty_register_driver(driver);
	if (res) {
		pr_err("failed to register nfcon tty driver\n");
		tty_driver_kref_put(driver);
		tty_port_destroy(&nfcon_tty_port);
		return res;
	}

	nfcon_tty_driver = driver;

	if (!console_is_registered(&nf_console))
		register_console(&nf_console);

	return 0;
}

static void __exit nfcon_exit(void)
{
	unregister_console(&nf_console);
	tty_unregister_driver(nfcon_tty_driver);
	tty_driver_kref_put(nfcon_tty_driver);
	tty_port_destroy(&nfcon_tty_port);
}

module_init(nfcon_init);
module_exit(nfcon_exit);

MODULE_LICENSE("GPL");
