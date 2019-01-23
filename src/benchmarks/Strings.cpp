#include "map_defines.h"

#include "bench.h"

#include <string>

void strfindbench(Bench& bench, size_t len) {
	auto& rng = bench.rng();

	std::string query(len, 'x');

	size_t testHash = 0;
	bench.beginMeasure();
	{
		Map<std::string, int> map;
		for (size_t n = 0; n < 1'000'000; ++n) {
			*reinterpret_cast<uint64_t*>(&query[0]) = rng(1'000'000);

			map[query] = rng();
			for (size_t q = 0; q < 100; ++q) {
				*reinterpret_cast<uint64_t*>(&query[0]) = rng(100'000);
				if (map.find(query) == map.end()) {
					++testHash;
				}
			}
		}
		bench.event("done");
		hash_combine(testHash, map.size());
	}
	bench.event("dtor");
	bench.endMeasure();
	bench.result(0, testHash);
}

static void StringFind1000(Bench& bench) {
	bench.title("StringFind1000");
	strfindbench(bench, 1000);
}
static void StringFind8(Bench& bench) {
	bench.title("StringFind8");
	strfindbench(bench, 8);
}

static void String50PercentDistinct(Bench& bench) {
	bench.title("String50PercentDistinct");
	static size_t const n = 50'000'000;
	size_t const max_rng = n / 2;

	auto& rng = bench.rng();

	size_t checksum = 0;
	bench.beginMeasure();
	{
		Map<std::string, size_t> map;
		std::string query(20, 'x');
		for (size_t i = 0; i < n; ++i) {
			*reinterpret_cast<uint64_t*>(&query[0]) = rng(max_rng);
			checksum += ++map[query];
		}
		bench.event("50% distinct");
	}
	bench.event("dtor");
	bench.endMeasure();
	bench.result(0x61600192900f6a54, checksum);
}

static BenchRegister reg(String50PercentDistinct);