#include "bench.h"

#if 0
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

static void String25PercentDistinct20(Bench& bench) {
	bench.title("String25PercentDistinct20");
	size_t const n = 50'000'000;
	size_t const max_rng = n / 4;
	size_t const string_len = 20;

	auto& rng = bench.rng();

	size_t checksum = 0;
	bench.beginMeasure();
	{
		Map<std::string, size_t> map;
		std::string query(string_len, 'x');

		// modifying the last bytes
		for (size_t i = 0; i < n; ++i) {
			*reinterpret_cast<uint64_t*>(&query[string_len - 8]) = rng(max_rng);
			checksum += ++map[query];
		}
		bench.event("25% distinct");
	}
	bench.event("dtor");
	bench.endMeasure();
	bench.result(0x6ce1e2c43532edc3, checksum);
}

static void String25PercentDistinct1000(Bench& bench) {
	bench.title("String25PercentDistinct1000");
	size_t const n = 25'000'000;
	size_t const max_rng = n / 4;
	size_t const string_len = 500;

	auto& rng = bench.rng();

	size_t checksum = 0;
	bench.beginMeasure();
	{
		Map<std::string, size_t> map;
		std::string query(string_len, 'x');

		// modifying the last bytes
		for (size_t i = 0; i < n; ++i) {
			*reinterpret_cast<uint64_t*>(&query[string_len - 8]) = rng(max_rng);
			checksum += ++map[query];
		}
		bench.event("25% distinct");
	}
	bench.event("dtor");
	bench.endMeasure();
	bench.result(0xb96ead7ca2ab42c, checksum);
}

static BenchRegister reg(String25PercentDistinct20, String25PercentDistinct1000);
#endif