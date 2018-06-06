#include "map_defines.h"

#include "bench.h"

static void RandomInsertErase(Bench& bench) {
	bench.title("RandomInsertErase");
	bench.description("randomly inserts and erases int values");

	// setup
	Map<int, int> map;

	// time measured part
	bench.beginMeasure();
	for (size_t n = 10000; n < 20000; ++n) {
		for (size_t i = 0; i < 10000; ++i) {
			map[bench.rng(n)] = i;
			map.erase(bench.rng(n));
		}
	}
	bench.endMeasure();

	// result map status
	bench.result(mapHash(map));
}

static void RandomInsertEraseStrings(Bench& bench) {
	bench.title("RandomInsertEraseStrings: map with strings");

	// setup
	Map<std::string, int> map;

	std::string str(100, 'x');

	// time measured part
	bench.beginMeasure();
	for (size_t n = 10000; n < 20000; ++n) {
		for (size_t i = 0; i < 10000; ++i) {
			*reinterpret_cast<uint64_t*>(&str[20]) = bench.rng(n);
			map[str] = i;
			*reinterpret_cast<uint64_t*>(&str[20]) = bench.rng(n);
			map.erase(str);
		}
	}
	bench.endMeasure();

	// result map status
	bench.result(mapHash(map));
}

static BenchRegister reg(RandomInsertErase, RandomInsertEraseStrings);