#include "map_defines.h"

#include "RandomBool.h"
#include "bench.h"

template <class Key, class Val>
size_t run(size_t num_iters, Bench& bench) {
	auto& rng = bench.rng();

	Map<Key, Val> map;

	size_t found = 0;

	bench.beginMeasure();
	for (size_t iters = 0; iters < num_iters; ++iters) {
		// recover rng state for insertion
		for (int j = 0; j < 100'000; ++j) {
			map.emplace(static_cast<Key>(rng()), static_cast<Val>(j));
		}
		bench.event("inserted 100.000");

		for (size_t n = 0; n < 100'000'000; ++n) {
			auto it = map.find(static_cast<Key>(rng()));
			if (it != map.end()) {
				++found;
			}
		}
		bench.event("found 100.000.000");
	}
	bench.event("done");
	bench.endMeasure();

	hash_combine(found, mapHash(map));
	return found;
}

static void RandomFindNonexisting(Bench& bench) {
	bench.title("RandomFindNonexisting");
	auto result = run<uint32_t, uint32_t>(8, bench);
	bench.result(0x881ae8dd442c712b, result);
}

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