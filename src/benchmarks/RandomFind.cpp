#include "map_defines.h"

#include "bench.h"

template <class Generator>
void randomFindExisting(Bench& bench, Generator const& generator) {
	auto& rng = bench.rng();

	Map<uint32_t, uint32_t> map;

	// time measured part
	bench.beginMeasure();

	auto const begin_state = rng.state();

	auto insertion_state = rng.state();
	auto find_state = rng.state();

	size_t found = 0;
	size_t not_found = 0;

	for (size_t iters = 0; iters < 20; ++iters) {
		// recover rng state for insertion
		rng.state(insertion_state);
		for (int j = 0; j < 100'000; ++j) {
			map[generator(rng)] = j;
		}
		bench.event("inserted 100.000");
		insertion_state = rng.state();

		rng.state(find_state);
		for (size_t n = 1; n < 100'000'000; ++n) {
			auto it = map.find(generator(rng));
			if (it == map.end()) {
				++not_found;
				// element not found - set state back to begin so we look for only existing entries
				rng.state(begin_state);
			} else {
				found += it->second;
			}
		}
		bench.event("found 100.000.000");

		// remember where we stopped searching
		find_state = rng.state();
	}
	bench.endMeasure();

	std::cout << found << " found, " << not_found << " not found" << std::endl;

	hash_combine(found, mapHash(map));

	bench.result(found);
}

static void RandomFindExisting(Bench& bench) {
	bench.title("RandomFindExisting");
	bench.description("randomly find existing values");

	randomFindExisting(bench, [](XoRoShiRo128Plus& rng) { return rng(); });
}

static void RandomFindNonExisting(Bench& bench) {
	bench.title("RandomFindNonExisting");
	bench.description("randomly find existing values");

	auto& rng = bench.rng();

	Map<uint32_t, uint32_t> map;

	size_t found = 0;
	size_t not_found = 0;

	bench.beginMeasure();
	for (size_t iters = 0; iters < 20; ++iters) {
		// recover rng state for insertion
		for (int j = 0; j < 100'000; ++j) {
			map[rng()] = j;
		}
		bench.event("inserted 100.000");

		for (size_t n = 1; n < 100'000'000; ++n) {
			auto it = map.find(rng());
			if (it == map.end()) {
				++not_found;
			} else {
				found += it->second;
			}
		}
		bench.event("found 100.000.000");
	}
	bench.event("done");
	bench.endMeasure();

	std::cout << found << " found, " << not_found << " not found" << std::endl;

	hash_combine(found, mapHash(map));

	bench.result(found);
}

static BenchRegister reg(RandomFindExisting, RandomFindNonExisting);