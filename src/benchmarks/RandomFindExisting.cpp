#include "map_defines.h"

#include "bench.h"

#include <list>
#include <map>
#include <memory>
#include <vector>

template <class Key, class Val, class Generator>
void run(Bench& bench, Generator const& generator) {
	auto& rng = bench.rng();

	Map<Key, Val> map;

	// time measured part
	bench.beginMeasure();

	auto const begin_state = rng.state();

	auto insertion_state = rng.state();
	auto find_state = rng.state();

	size_t found = 0;

	for (size_t iters = 0; iters < 20; ++iters) {
		// recover rng state for insertion
		rng.state(insertion_state);
		for (int j = 0; j < 100'000; ++j) {
			map.emplace(generator(rng), j);
		}
		bench.event("inserted 100.000");
		insertion_state = rng.state();

		rng.state(find_state);
		for (size_t n = 0; n < 100'000'000; ++n) {
			auto it = map.find(generator(rng));
			if (it != map.end()) {
				++found;
			} else {
				// element not found - set state back to begin so we look for only existing entries
				rng.state(begin_state);
			}
		}
		bench.event("found 100.000.000");

		// remember where we stopped searching
		find_state = rng.state();
	}
	bench.event("done");
	bench.endMeasure();

	hash_combine(found, mapHash(map));
	bench.result(found);
}

static void RandomFindExisting(Bench& bench) {
	bench.title("RandomFindExisting");
	bench.description("randomly find existing values");

	run<uint32_t, uint32_t>(bench, [](XoRoShiRo128Plus& rng) { return rng(); });
}

struct Data {
	Data(int i) {}
	Data() {}
	std::list<int> a;
	std::vector<int> b;
	std::map<int, int> c;
	std::shared_ptr<int> d;
};

uint64_t hash_value(Data const& data) {
	// just hash of the address
	return static_cast<uint64_t>(&data - static_cast<Data*>(0));
}

static void RandomFindExistingBig(Bench& bench) {
	bench.title("RandomFindExistingBig");

	run<uint32_t, Data>(bench, [](XoRoShiRo128Plus& rng) { return rng(); });
}

static BenchRegister reg(RandomFindExisting, RandomFindExistingBig);