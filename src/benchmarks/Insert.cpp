#include "bench.h"

static void InsertHugeInt(Bench& bench) {
	bench.title("InsertHugeInt");
	auto& rng = bench.rng();

	size_t result = 0;
	bench.beginMeasure();
	{
		Map<int, int> map;
		for (size_t n = 0; n < 100'000'000; ++n) {
			map[rng()];
		}
		result += map.size();
		bench.event("inserted");

		map.clear();
		bench.event("cleared");
		for (size_t n = 0; n < 100'000'000; ++n) {
			map[rng()];
		}
		result += map.size();
		bench.event("inserted");
	}
	bench.event("destructed");
	bench.endMeasure();

	// result map status
	bench.result(0x77ca8aa5816674af, result);
}

static BenchRegister reg(InsertHugeInt);

#if 0
//#if 1
// static BenchRegister reg(InsertHugeInt);
//#else
struct BigData {
	/*
	std::shared_ptr<int> a;
	std::map<int, int> b;
	std::vector<double> c;
	std::unique_ptr<int> d;
	*/
	std::fstream f;
};

static void InsertHugeBigData(Bench& bench) {
	Map<int, std::fstream> map;
	/*
	bench.title("InsertHugeBigData");
	auto& rng = bench.rng();

	size_t result = 0;
	bench.beginMeasure();
	{
		Map<int, BigData> map;
		for (size_t n = 0; n < 4'000'000; ++n) {
			map[rng()];
		}
		result += map.size();
		bench.event("inserted");

		map.clear();
		bench.event("cleared");
		for (size_t n = 0; n < 4'000'000; ++n) {
			map[rng()];
		}
		result += map.size();
		bench.event("inserted");
	}
	bench.event("destructed");
	bench.endMeasure();

	// result map status

	bench.result(0x6b7621434f3cefb6, result);
	*/
}

static BenchRegister reg(InsertHugeInt, InsertHugeBigData);
//#endif
#endif