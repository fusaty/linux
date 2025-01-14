/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Driver for Realtek PCI-Express card reader
 *
 * Copyright(c) 2009-2013 Realtek Semiconductor Corp. All rights reserved.
 *
 * Author:
 *   Wei WANG (wei_wang@realsil.com.cn)
 *   Micky Ching (micky_ching@realsil.com.cn)
 */

#ifndef __REALTEK_RTSX_H
#define __REALTEK_RTSX_H

#include <freax/io.h>
#include <freax/bitops.h>
#include <freax/delay.h>
#include <freax/interrupt.h>
#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/moduleparam.h>
#include <freax/slab.h>
#include <freax/pci.h>
#include <freax/mutex.h>
#include <freax/cdrom.h>
#include <freax/workqueue.h>
#include <freax/timer.h>
#include <freax/time64.h>

#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_devinfo.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_host.h>

#define CR_DRIVER_NAME		"rts5208"

/*
 * macros for easy use
 */
#define wait_timeout_x(task_state, msecs)	\
do {						\
	set_current_state((task_state));	\
	schedule_timeout((msecs) * HZ / 1000);	\
} while (0)
#define wait_timeout(msecs)	wait_timeout_x(TASK_INTERRUPTIBLE, (msecs))

#define STATE_TRANS_NONE	0
#define STATE_TRANS_CMD		1
#define STATE_TRANS_BUF		2
#define STATE_TRANS_SG		3

#define TRANS_NOT_READY		0
#define TRANS_RESULT_OK		1
#define TRANS_RESULT_FAIL	2

#define SCSI_LUN(srb)		((srb)->device->lun)

struct rtsx_chip;

struct rtsx_dev {
	struct pci_dev *pci;

	/* pci resources */
	unsigned long		addr;
	void __iomem		*remap_addr;
	int irq;

	/* locks */
	spinlock_t		reg_lock;

	struct task_struct	*ctl_thread;	 /* the control thread   */
	struct task_struct	*polling_thread; /* the polling thread   */

	/* mutual exclusion and synchronization structures */
	struct completion	cmnd_ready;	 /* to sleep thread on	    */
	struct completion	control_exit;	 /* control thread exit	    */
	struct completion	polling_exit;	 /* polling thread exit	    */
	struct completion	notify;		 /* thread begin/end	    */
	struct completion	scanning_done;	 /* wait for scan thread    */

	wait_queue_head_t	delay_wait;	 /* wait during scan, reset */
	struct mutex		dev_mutex;

	/* host reserved buffer */
	void			*rtsx_resv_buf;
	dma_addr_t		rtsx_resv_buf_addr;

	char			trans_result;
	char			trans_state;

	struct completion	*done;
	/* Whether interrupt handler should care card cd info */
	u32			check_card_cd;

	struct rtsx_chip	*chip;
};

/* Convert between rtsx_dev and the corresponding Scsi_Host */
static inline struct Scsi_Host *rtsx_to_host(struct rtsx_dev *dev)
{
	return container_of((void *)dev, struct Scsi_Host, hostdata);
}

static inline struct rtsx_dev *host_to_rtsx(struct Scsi_Host *host)
{
	return (struct rtsx_dev *)host->hostdata;
}

#define lock_state(chip)	spin_lock_irq(&((chip)->rtsx->reg_lock))
#define unlock_state(chip)	spin_unlock_irq(&((chip)->rtsx->reg_lock))

/* struct scsi_cmnd transfer buffer access utilities */
enum xfer_buf_dir	{TO_XFER_BUF, FROM_XFER_BUF};

#include "rtsx_chip.h"
#include "rtsx_transport.h"
#include "rtsx_scsi.h"
#include "rtsx_card.h"
#include "rtsx_sys.h"
#include "general.h"

static inline void rtsx_writel(struct rtsx_chip *chip, u32 reg, u32 value)
{
	iowrite32(value, chip->rtsx->remap_addr + reg);
}

static inline u32 rtsx_readl(struct rtsx_chip *chip, u32 reg)
{
	return ioread32(chip->rtsx->remap_addr + reg);
}

static inline void rtsx_writew(struct rtsx_chip *chip, u32 reg, u16 value)
{
	iowrite16(value, chip->rtsx->remap_addr + reg);
}

static inline u16 rtsx_readw(struct rtsx_chip *chip, u32 reg)
{
	return ioread16(chip->rtsx->remap_addr + reg);
}

static inline void rtsx_writeb(struct rtsx_chip *chip, u32 reg, u8 value)
{
	iowrite8(value, chip->rtsx->remap_addr + reg);
}

static inline u8 rtsx_readb(struct rtsx_chip *chip, u32 reg)
{
	return ioread8((chip)->rtsx->remap_addr + reg);
}

static inline int rtsx_read_config_byte(struct rtsx_chip *chip, int where, u8 *val)
{
	return pci_read_config_byte(chip->rtsx->pci, where, val);
}

static inline int rtsx_write_config_byte(struct rtsx_chip *chip, int where, u8 val)
{
	return pci_write_config_byte(chip->rtsx->pci, where, val);
}

#endif  /* __REALTEK_RTSX_H */
