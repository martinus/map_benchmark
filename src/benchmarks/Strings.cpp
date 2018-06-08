#include "map_defines.h"

#include "bench.h"

#include <string>

void strfindbench(Bench& bench, size_t len) {
	Map<std::string, int> map;

	std::string query(len, 'x');

	size_t testHash = 0;
	bench.beginMeasure();
	for (size_t n = 0; n < 1'000'000; ++n) {
		*reinterpret_cast<uint64_t*>(&query[0]) = bench.rng(1'000'000);

		map[query] = bench.rng();
		for (size_t q = 0; q < 100; ++q) {
			*reinterpret_cast<uint64_t*>(&query[0]) = bench.rng(100'000);
			if (map.find(query) == map.end()) {
				++testHash;
			}
		}
	}
	bench.endMeasure();

	hash_combine(testHash, mapHash(map));
	bench.result(testHash);
}

static void StringFind1000(Bench& bench) {
	bench.title("StringFind1000");
	strfindbench(bench, 1000);
}
static void StringFind8(Bench& bench) {
	bench.title("StringFind8");
	strfindbench(bench, 8);
}

static BenchRegister reg(StringFind1000, StringFind8);