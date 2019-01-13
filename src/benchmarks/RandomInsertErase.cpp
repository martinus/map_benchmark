#include "map_defines.h"

#include "bench.h"

template <int RngShift>
void run(Bench& bench) {
	auto& rng = bench.rng();

	// time measured part
	size_t verifier = 0;
	bench.beginMeasure();
	{
		Map<uint64_t, uint64_t> map;

		for (size_t n = 1; n < 10'000; ++n) {
			if ((n % 200) == 0) {
				bench.event("");
			}
			for (size_t i = 0; i < 10'000; ++i) {
				map[rng(n)] = i;
				verifier += map.erase(rng(n));
			}
		}
		bench.event("destructing");
		hash_combine(verifier, map.size());
	}
	bench.event("done");
	bench.endMeasure();

	// result map status
	bench.result(verifier);
}

static void RandomInsertErase(Bench& bench) {
	bench.title("RandomInsertErase");
	bench.description("randomly inserts and erases int values");
	run<0>(bench);
}
static void RandomInsertEraseShifted(Bench& bench) {
	bench.title("RandomInsertEraseShifted");
	bench.description(
		"randomly inserts and erases int values, but these values are leftshifted 16 bit, thus testing the behavior with a bad hash function.");
	run<4>(bench);
}

static void RandomInsertEraseStrings(Bench& bench) {
	bench.title("RandomInsertEraseStrings");
	auto& rng = bench.rng();

	// setup
	Map<std::string, int> map;

	std::string str(100, 'x');

	// time measured part
	bench.beginMeasure();
	for (size_t n = 10000; n < 20000; ++n) {
		for (size_t i = 0; i < 10000; ++i) {
			*reinterpret_cast<uint64_t*>(&str[20]) = rng(n);
			map[str] = i;
			*reinterpret_cast<uint64_t*>(&str[20]) = rng(n);
			map.erase(str);
		}
	}
	bench.endMeasure();

	// result map status
	bench.result(mapHash(map));
}

static BenchRegister reg(RandomInsertErase, RandomInsertEraseShifted, RandomInsertEraseStrings);
