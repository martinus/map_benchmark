#include "RandomBool.h"
#include "bench.h"

#include <iomanip>

template <size_t NumRandom>
static void RandomFind(Bench& bench) {
	size_t constexpr NumTotal = 4;
	size_t constexpr NumSequential = NumTotal - NumRandom;

	size_t constexpr NumInserts = 1000000;
	size_t constexpr NumFindsPerIter = 1000 * NumTotal;

	std::stringstream ss;
	ss << "RandomFind " << std::setw(3) << (NumSequential * 100 / NumTotal) << "% find success";
	bench.title(ss.str());
	auto& rng = bench.rng();
	RandomBool rbool;

	size_t num_found = 0;

	std::array<bool, NumTotal> insertRandom = {false};
	for (size_t i = 0; i < NumRandom; ++i) {
		insertRandom[i] = true;
	}

	bench.beginMeasure();
	{
		Map<size_t, size_t> map;
		size_t i = 0;
		do {
			// insert NumTotal entries: some random, some sequential.
			std::shuffle(insertRandom.begin(), insertRandom.end(), rng);
			for (bool isRandomToInsert : insertRandom) {
				if (isRandomToInsert) {
					map.emplace(static_cast<size_t>(rng()), i);
				} else {
					map.emplace(i, i);
				}
				++i;
			}

			for (size_t j = 0; j < NumFindsPerIter; ++j) {
				num_found += map.count(static_cast<size_t>(rng(i)));
			}
		} while (i < NumInserts);
		bench.event("done");
	}
	bench.event("dtor");
	bench.endMeasure();

	// 0%, 25%, 50%, 75%, 100%
	uint64_t results[] = {0x3163e88fd0795ae3, 0xca19937be09d1cba, 0xcacf1231545863ed, 0x290e087fc88f135a, 0xa520660353372c76};

	bench.result(results[NumSequential], num_found);
}

static BenchRegister reg(RandomFind<0>, RandomFind<1>, RandomFind<2>, RandomFind<3>, RandomFind<4>);