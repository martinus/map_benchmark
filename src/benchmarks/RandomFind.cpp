#include "map_defines.h"

#include "bench.h"

static const size_t NumEntriesStart = 100'000;
static const size_t NumEntriesStop = 200'000;

template <int Offset>
void randomFindBench(Bench& bench) {
	Map<int, int> map;

	int i;
	while (i < NumEntriesStart) {
		map[i + Offset] = i;
		++i;
	}

	// time measured part
	size_t found = 0;
	bench.beginMeasure();

	while (i < NumEntriesStop) {
		for (int j = 0; j < 100; ++j) {
			map[i + Offset] = i;
			++i;
		}

		for (size_t n = 1; n < 100'000; ++n) {
			if (map.find(bench.rng(i)) != map.end()) {
				++found;
			}
		}
	}
	bench.endMeasure();

	hash_combine(found, mapHash(map));
	bench.result(found);
}

static void RandomFindExisting(Bench& bench) {
	bench.title("RandomFindExisting");
	bench.description("randomly find existing values");
	randomFindBench<0>(bench);
}

static void RandomFindNonExisting(Bench& bench) {
	bench.title("RandomFindNonExisting");
	bench.description("randomly find existing values");

	randomFindBench<(std::numeric_limits<int>::max)() / 2>(bench);
}

static BenchRegister reg(RandomFindExisting, RandomFindNonExisting);