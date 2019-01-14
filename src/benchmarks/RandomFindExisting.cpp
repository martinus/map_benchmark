#include "map_defines.h"

#include "bench.h"

#include <list>
#include <map>
#include <memory>
#include <vector>

template <class Key, class Val>
size_t run(size_t num_iters, Bench& bench) {
	auto& rng = bench.rng();

	// time measured part
	bench.beginMeasure();
	size_t found = 0;

	{
		Map<Key, Val> map;

		auto const begin_state = rng.state();

		auto insertion_state = rng.state();
		auto find_state = rng.state();

		for (size_t iters = 0; iters < num_iters; ++iters) {
			// recover rng state for insertion
			rng.state(insertion_state);
			for (int j = 0; j < 100'000; ++j) {
				map.emplace(rng(), j);
			}
			bench.event("inserted 100.000");
			insertion_state = rng.state();

			rng.state(find_state);
			for (size_t n = 0; n < 100'000'000; ++n) {
				auto it = map.find(rng());
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
		hash_combine(found, map.size());
		bench.event("destructing");
	}
	bench.event("done");
	bench.endMeasure();
	return found;
}

static void RandomFindExisting(Bench& bench) {
	bench.title("RandomFindExisting");
	bench.description("randomly find existing values");
	auto result = run<uint32_t, uint32_t>(8, bench);
	bench.result(0x8a01d73e63b75916, result);
}

struct Data {
	Data(int i)
		: mI(i) {}
	Data() = default;

	std::list<int> a;
	std::vector<int> b;
	std::map<int, int> c;
	std::shared_ptr<int> d;
	int mI;
};

uint64_t hash_value(Data const& data) {
	// just hash of the address
	return static_cast<uint64_t>(&data - static_cast<Data*>(0));
}

static void RandomFindExistingBig(Bench& bench) {
	bench.title("RandomFindExistingBig");
	bench.description("randomly find existing values");
	auto result = run<uint32_t, Data>(5, bench);
	bench.result(0x24dd9e7eafe6d413, result);
}

static void CollisionFinder(Bench& bench) {
	bench.title("CollisionFinder");
	bench.description("finds numbers with good collision properties for multiplication factor");

	auto& rng = bench.rng();
	rng.seed();

	// make sure we get an odd number
	size_t minCol = (std::numeric_limits<size_t>::max)();
	static size_t const arraySizeShift = 12;

	while (true) {
		auto factor = rng() | 1;

		size_t col = 0;

		std::array<size_t, (1 << arraySizeShift)> taken = {};

		for (int shift = 0; shift < 32; ++shift) {
			for (size_t i = 0; i < taken.size(); ++i) {
				auto num = i << shift;
				auto idx = (factor * num) >> (64 - arraySizeShift);
				if (++taken[idx] > col) {
					col = taken[idx];
					if (col >= minCol) {
						i = taken.size();
						shift = 32;
						break;
					}
				}
			}
		}

		if (col < minCol) {
			minCol = col;
			std::cout << col << " " << factor << std::endl;
		}
	}
}

static BenchRegister reg(RandomFindExisting, RandomFindExistingBig);