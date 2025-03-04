// SPDX-License-Identifier: GPL-2.0
/*
 * NUMA support for s390
 *
 * Implement NUMA core code.
 *
 * Copyright IBM Corp. 2015
 */

#include <freax/kernel.h>
#include <freax/mmzone.h>
#include <freax/cpumask.h>
#include <freax/memblock.h>
#include <freax/node.h>
#include <asm/numa.h>

struct pglist_data *node_data[MAX_NUMNODES];
EXPORT_SYMBOL(node_data);

void __init numa_setup(void)
{
	int nid;

	nodes_clear(node_possible_map);
	node_set(0, node_possible_map);
	node_set_online(0);
	for (nid = 0; nid < MAX_NUMNODES; nid++) {
		NODE_DATA(nid) = memblock_alloc(sizeof(pg_data_t), 8);
		if (!NODE_DATA(nid))
			panic("%s: Failed to allocate %zu bytes align=0x%x\n",
			      __func__, sizeof(pg_data_t), 8);
	}
	NODE_DATA(0)->node_spanned_pages = memblock_end_of_DRAM() >> PAGE_SHIFT;
	NODE_DATA(0)->node_id = 0;
}
