#include "bench.h"

static void RandomDistinct2(Bench& bench) {
	bench.title("RandomDistinct2");
	static size_t const n = 50'000'000;

	auto& rng = bench.rng();

	int checksum = 0;
	bench.beginMeasure();
	{
		size_t const max_rng = n / 20;
		Map<int, int> map;
		for (size_t i = 0; i < n; ++i) {
			checksum += ++map[static_cast<int>(rng(max_rng))];
		}
		bench.event("5% distinct");
	}
	bench.event("dtor");

	{
		size_t const max_rng = n / 4;
		Map<int, int> map;
		for (size_t i = 0; i < n; ++i) {
			checksum += ++map[static_cast<int>(rng(max_rng))];
		}
		bench.event("25% distinct");
	}
	bench.event("dtor");

	{
		size_t const max_rng = n / 2;
		Map<int, int> map;
		for (size_t i = 0; i < n; ++i) {
			checksum += ++map[static_cast<int>(rng(max_rng))];
		}
		bench.event("50% distinct");
	}
	bench.event("dtor");

	{
		Map<int, int> map;
		for (size_t i = 0; i < n; ++i) {
			checksum += ++map[static_cast<int>(rng())];
		}
		bench.event("100% distinct");
	}
	bench.event("dtor");
	bench.endMeasure();
	bench.result(0xb054cacf033c42bd, checksum);
}

static BenchRegister reg(RandomDistinct2);