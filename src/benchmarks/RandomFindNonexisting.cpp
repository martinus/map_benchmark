#include "map_defines.h"

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

static BenchRegister reg(RandomFindNonexisting);