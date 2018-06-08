#include "map_defines.h"

#include "bench.h"

static void InsertHugeInt(Bench& bench) {
	bench.title("InsertHugeInt");

	size_t result = 0;
	bench.beginMeasure();
	{
		Map<int, int> map;
		for (size_t n = 0; n < 50'000'000; ++n) {
			map[bench.rng()];
		}
		result += map.size();
		bench.event("inserted");

		map.clear();
		bench.event("cleared");
		for (size_t n = 0; n < 50'000'000; ++n) {
			map[bench.rng()];
		}
		result += map.size();
		bench.event("inserted");
	}
	bench.event("destructed");
	bench.endMeasure();

	// result map status
	bench.result(result);
}

struct BigData {
	std::shared_ptr<int> a;
	std::map<int, int> b;
	std::vector<double> c;
	std::unique_ptr<int> d;
	std::fstream f;
};

static void InsertHugeBigData(Bench& bench) {
	bench.title("InsertHugeBigData");

	size_t result = 0;
	bench.beginMeasure();
	{
		Map<int, BigData> map;
		for (size_t n = 0; n < 1'000'000; ++n) {
			map[bench.rng()];
		}
		result += map.size();

		map.clear();
		for (size_t n = 0; n < 1'000'000; ++n) {
			map[bench.rng()];
		}
		result += map.size();
	}
	bench.endMeasure();

	// result map status
	bench.result(result);
}

static BenchRegister reg(InsertHugeInt, InsertHugeBigData);