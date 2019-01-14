#include "map_defines.h"

#include "bench.h"

static void IterateIntegers(Bench& bench) {
	bench.title("IterateIntegers");
	auto& rng = bench.rng();

	int result = 0;
	{
		Map<int, int> map;

		bench.beginMeasure();
		for (size_t n = 0; n < 65000; ++n) {
			map[rng()] = n;
			for (auto const& keyVal : map) {
				result += keyVal.first;
				result += keyVal.second;
			}
		}
		bench.event("destructing");
	}

	bench.event("done");
	bench.endMeasure();

	// result map status
	bench.result(0x522cacb81dde2db6, result);
}

static void IterateClearedWithSingleElement(Bench& bench) {
	bench.title("IterateClearedWithSingleElement");
	auto& rng = bench.rng();

	Map<size_t, int> map;
	for (size_t i = 0; i < 100000; ++i) {
		map[i];
	}

	map.clear();

	// now there is just a single element in the map
	map[rng()] = rng();

	int result = 0;
	bench.beginMeasure();
	for (size_t n = 0; n < 30000; ++n) {
		for (auto const& keyVal : map) {
			result += keyVal.first;
			result += keyVal.second;
		}
	}
	bench.endMeasure();

	// result map status
	bench.result(0, result);
}

// , IterateClearedWithSingleElement
static BenchRegister reg(IterateIntegers);