#include "map_defines.h"

#include "bench.h"

template <class Key, class Val>
void run(Bench& bench) {
	auto& rng = bench.rng();

	Map<Key, Val> map;

	size_t found = 0;

	bench.beginMeasure();
	for (size_t iters = 0; iters < 20; ++iters) {
		// recover rng state for insertion
		for (int j = 0; j < 100'000; ++j) {
			map.emplace(rng(), j);
		}
		bench.event("inserted 100.000");

		for (size_t n = 0; n < 100'000'000; ++n) {
			auto it = map.find(rng());
			if (it != map.end()) {
				++found;
			}
		}
		bench.event("found 100.000.000");
	}
	bench.event("done");
	bench.endMeasure();

	hash_combine(found, mapHash(map));
	bench.result(found);
}

static void RandomFindNonexisting(Bench& bench) {
	bench.title("RandomFindNonexisting");
	run<uint32_t, uint32_t>(bench);
}

static BenchRegister reg(RandomFindNonexisting);