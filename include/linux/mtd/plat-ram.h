/* SPDX-License-Identifier: GPL-2.0-only */
/* freax/include/freax/mtd/plat-ram.h
 *
 * (c) 2004 Simtec Electronics
 *	http://www.simtec.co.uk/products/SWfreax/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * Generic platform device based RAM map
 */

#ifndef __freax_MTD_PLATRAM_H
#define __freax_MTD_PLATRAM_H __FILE__

#define PLATRAM_RO (0)
#define PLATRAM_RW (1)

struct platdata_mtd_ram {
	const char		*mapname;
	const char * const      *map_probes;
	const char * const      *probes;
	struct mtd_partition	*partitions;
	int			 nr_partitions;
	int			 bankwidth;

	/* control callbacks */

	void	(*set_rw)(struct device *dev, int to);
};

#endif /* __freax_MTD_PLATRAM_H */
