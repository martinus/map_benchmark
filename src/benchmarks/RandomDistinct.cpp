#include "map_defines.h"

#include "bench.h"

static void RandomDistinct(Bench& bench) {
	bench.title("RandomDistinct");
	bench.description("25% distinct entries. Adapted from https://github.com/attractivechaos/udb2");

	auto& rng = bench.rng();

	size_t checksum = 0;
	size_t const upper = 70'000'000;
	size_t const lower = 10'000'000;
	size_t const num_steps = 7;
	size_t const step_width = (upper - lower) / num_steps;

	bench.beginMeasure();
	for (size_t n = lower; n <= upper; n += step_width) {
		size_t const max_rng = n / 4;
		{
			bench.event("start");
			Map<int, int> map;
			for (size_t i = 0; i < n; ++i) {
				checksum += ++map[static_cast<int>(rng(max_rng))];
			}
			bench.event("finish");
		}
	}
	bench.event("done");
	bench.endMeasure();
	bench.result(0xeb69f3ce52c0a705, checksum);
}

static BenchRegister reg(RandomDistinct);