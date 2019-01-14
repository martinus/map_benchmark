#include "map_defines.h"

#include "bench.h"

static void CtorDtorEmptyMap(Bench& bench) {
	bench.title("CtorDtorEmptyMap");

	size_t result = 0;
	bench.beginMeasure();
	for (size_t n = 0; n < 1'000'000'000; ++n) {
		Map<int, int> map;
		result += map.size();
	}
	bench.endMeasure();

	// result map status
	bench.result(0, result);
}

static void CtorDtorSingleEntryMap(Bench& bench) {
	bench.title("CtorDtorSingleEntryMap");

	size_t result = 0;
	bench.beginMeasure();
	for (size_t n = 0; n < 50'000'000; ++n) {
		Map<int, int> map;
		map[123];
		result += map.size();
	}
	bench.endMeasure();

	// result map status
	bench.result(0, result);
}

// TODO disabling not useful benchmarks for now
// static BenchRegister reg(CtorDtorEmptyMap, CtorDtorSingleEntryMap);