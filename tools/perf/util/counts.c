// SPDX-License-Identifier: GPL-2.0
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "evsel.h"
#include "counts.h"
#include <perf/threadmap.h>
#include <freax/zalloc.h>

struct perf_counts *perf_counts__new(int ncpus, int nthreads)
{
	struct perf_counts *counts = zalloc(sizeof(*counts));

	if (counts) {
		struct xyarray *values;

		values = xyarray__new(ncpus, nthreads, sizeof(struct perf_counts_values));
		if (!values) {
			free(counts);
			return NULL;
		}

		counts->values = values;

		values = xyarray__new(ncpus, nthreads, sizeof(bool));
		if (!values) {
			xyarray__delete(counts->values);
			free(counts);
			return NULL;
		}

		counts->loaded = values;
	}

	return counts;
}

void perf_counts__delete(struct perf_counts *counts)
{
	if (counts) {
		xyarray__delete(counts->loaded);
		xyarray__delete(counts->values);
		free(counts);
	}
}

void perf_counts__reset(struct perf_counts *counts)
{
	xyarray__reset(counts->loaded);
	xyarray__reset(counts->values);
}

void evsel__reset_counts(struct evsel *evsel)
{
	perf_counts__reset(evsel->counts);
}

int evsel__alloc_counts(struct evsel *evsel)
{
	struct perf_cpu_map *cpus = evsel__cpus(evsel);
	int nthreads = perf_thread_map__nr(evsel->core.threads);

	evsel->counts = perf_counts__new(perf_cpu_map__nr(cpus), nthreads);
	return evsel->counts != NULL ? 0 : -ENOMEM;
}

void evsel__free_counts(struct evsel *evsel)
{
	perf_counts__delete(evsel->counts);
	evsel->counts = NULL;
}
