#include "RandomBool.h"
#include "bench.h"

static void RandomFind(Bench& bench) {
	bench.title("RandomFind");
	auto& rng = bench.rng();
	RandomBool rbool;

	size_t num_found = 0;
	bench.beginMeasure();
	{
		Map<size_t, size_t> map;
		for (size_t i = 1; i <= 1000000; ++i) {
			if (rbool(rng)) {
				map.emplace(i, i);
			}

			for (size_t j = 0; j < 1000; ++j) {
				num_found += map.count(static_cast<size_t>(rng(i)));
			}
		}
		bench.event("done");
	}
	bench.event("dtor");
	bench.endMeasure();
	bench.result(0xd8a88adc94b169d6, num_found);
}

static BenchRegister reg(RandomFind);