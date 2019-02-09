#include "bench.h"

template <int RngShift>
size_t run(size_t max_n, Bench& bench) {
	auto& rng = bench.rng();

	// time measured part
	size_t verifier = 0;
	bench.beginMeasure();
	{
		Map<uint64_t, uint64_t> map;

		for (size_t n = 2; n < max_n; ++n) {
			if ((n % 200) == 0) {
				bench.event("");
			}
			for (size_t i = 0; i < max_n; ++i) {
				map[rng(n) << RngShift] = i;
				verifier += map.erase(rng(n) << RngShift);
			}
		}
		bench.event("destructing");
		hash_combine(verifier, map.size());
	}
	bench.event("done");
	bench.endMeasure();
	return verifier;
}

static void RandomInsertErase(Bench& bench) {
	bench.title("RandomInsertErase");
	bench.description("randomly inserts and erases int values");
	auto result = run<0>(21000, bench);
	bench.result(0x658159d96eca6f73, result);
}
static void RandomInsertEraseShifted(Bench& bench) {
	bench.title("RandomInsertEraseShifted");
	bench.description(
		"randomly inserts and erases int values, but these values are leftshifted 16 bit, thus testing the behavior with a bad hash function.");
	auto result = run<4>(21000, bench);
	bench.result(0x658159d96eca6f73, result);
}

static BenchRegister reg(RandomInsertErase, RandomInsertEraseShifted);

#if 0
static void RandomInsertEraseStrings(Bench& bench) {
	bench.title("RandomInsertEraseStrings");
	auto& rng = bench.rng();

	// setup
	{
		Map<std::string, int> map;

		std::string str(100, 'x');

		// time measured part
		bench.beginMeasure();
		for (size_t n = 10000; n < 18000; ++n) {
			for (size_t i = 0; i < 10000; ++i) {
				*reinterpret_cast<uint64_t*>(&str[20]) = rng(n);
				map[str] = i;
				*reinterpret_cast<uint64_t*>(&str[20]) = rng(n);
				map.erase(str);
			}
		}
		bench.event("done");
		bench.result(0x2a7686d5d25cc104, mapHash(map));
		bench.event("map hashed");
	}
	bench.event("destructed");
	bench.endMeasure();

	// result map status
}
static BenchRegister reg(RandomInsertErase, RandomInsertEraseShifted, RandomInsertEraseStrings);
#endif
