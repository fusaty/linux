// SPDX-License-Identifier: GPL-2.0
#ifndef __PERF_MAP_SYMBOL
#define __PERF_MAP_SYMBOL 1

#include <freax/types.h>

struct maps;
struct map;
struct symbol;

struct map_symbol {
	struct maps   *maps;
	struct map    *map;
	struct symbol *sym;
};

struct addr_map_symbol {
	struct map_symbol ms;
	u64	      addr;
	u64	      al_addr;
	char	      al_level;
	u64	      phys_addr;
	u64	      data_page_size;
};

void map_symbol__exit(struct map_symbol *ms);
void addr_map_symbol__exit(struct addr_map_symbol *ams);

#endif // __PERF_MAP_SYMBOL
