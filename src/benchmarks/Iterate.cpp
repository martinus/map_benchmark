#include "map_defines.h"

#include "bench.h"

static void IterateIntegers(Bench& bench) {
	bench.title("IterateIntegers");

	Map<int, int> map;

	int result = 0;
	bench.beginMeasure();
	for (size_t n = 0; n < 25000; ++n) {
		map[bench.rng()] = n;
		for (auto const& keyVal : map) {
			result += keyVal.first;
			result += keyVal.second;
		}
	}
	bench.endMeasure();

	// result map status
	bench.result(result);
}

static void IterateClearedWithSingleElement(Bench& bench) {
	bench.title("IterateClearedWithSingleElement");

	Map<size_t, int> map;
	for (size_t i = 0; i < 100000; ++i) {
		map[i];
	}

	map.clear();

	// now there is just a single element in the map
	map[bench.rng()] = bench.rng();

	int result = 0;
	bench.beginMeasure();
	for (size_t n = 0; n < 25000; ++n) {
		for (auto const& keyVal : map) {
			result += keyVal.first;
			result += keyVal.second;
		}
	}
	bench.endMeasure();

	// result map status
	bench.result(result);
}

static BenchRegister reg(IterateIntegers, IterateClearedWithSingleElement);