#include "map_defines.h"

#include "bench.h"

static void RandomInsertErase(Bench& bench) {
	bench.title("RandomInsertErase");
	bench.description("randomly inserts and erases int values");
	auto& rng = bench.rng();

	// time measured part
	size_t verifier = 0;
	bench.beginMeasure();
	{
		Map<int, int> map;

		for (size_t n = 1; n < 100'000; ++n) {
			for (size_t i = 0; i < 10'000; ++i) {
				map[rng(n)] = i;
				verifier += map.erase(rng(n));
			}
		}
		bench.event("destructing");
		hash_combine(verifier, map.size());
	}
	bench.event("done");
	bench.endMeasure();

	// result map status
	bench.result(verifier);
}

static void RandomInsertEraseStrings(Bench& bench) {
	bench.title("RandomInsertEraseStrings: map with strings");
	auto& rng = bench.rng();

	// setup
	Map<std::string, int> map;

	std::string str(100, 'x');

	// time measured part
	bench.beginMeasure();
	for (size_t n = 10000; n < 20000; ++n) {
		for (size_t i = 0; i < 10000; ++i) {
			*reinterpret_cast<uint64_t*>(&str[20]) = rng(n);
			map[str] = i;
			*reinterpret_cast<uint64_t*>(&str[20]) = rng(n);
			map.erase(str);
		}
	}
	bench.endMeasure();

	// result map status
	bench.result(mapHash(map));
}

static BenchRegister reg(RandomInsertErase, RandomInsertEraseStrings);