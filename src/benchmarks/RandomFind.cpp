#include "RandomBool.h"
#include "bench.h"

#include <iomanip>

template <size_t NumRandom>
static void RandomFind(Bench& bench) {
	size_t constexpr NumTotal = 4;
	size_t constexpr NumSequential = NumTotal - NumRandom;

	size_t constexpr NumInserts = 1000000;
	size_t constexpr NumFindsPerIter = 1000 * NumTotal;

	// just multiply numbers with a small factor so we don't just sequentially insert numbers
	size_t constexpr NotSequentialFactor = 31;

	std::stringstream ss;
	ss << "RandomFind " << std::setw(3) << (NumSequential * 100 / NumTotal) << "% find success";
	bench.title(ss.str());
	auto& rng = bench.rng();

	size_t num_found = 0;

	std::array<bool, NumTotal> insertRandom = {false};
	for (size_t i = 0; i < NumRandom; ++i) {
		insertRandom[i] = true;
	}

	sfc64 anotherUnrelatedRng(987654321);
	auto const anotherUnrelatedRngInitialState = anotherUnrelatedRng.state();
	sfc64 findRng(anotherUnrelatedRngInitialState);

	bench.beginMeasure();
	{
		Map<size_t, size_t> map;
		size_t i = 0;
		size_t findCount = 0;
		do {
			// insert NumTotal entries: some random, some sequential.
			std::shuffle(insertRandom.begin(), insertRandom.end(), rng);
			for (bool isRandomToInsert : insertRandom) {
				auto val = anotherUnrelatedRng();
				if (isRandomToInsert) {
					map.emplace(rng() >> 32, i);
				} else {
					map.emplace(val >> 32, i);
				}
				++i;
			}

			for (size_t j = 0; j < NumFindsPerIter; ++j) {
				if (++findCount > i) {
					findCount = 0;
					findRng.state(anotherUnrelatedRngInitialState);
				}
				num_found += map.count(findRng() >> 32);
			}
		} while (i < NumInserts);
		bench.event("done");
	}
	bench.event("dtor");
	bench.endMeasure();

	// std::cout << num_found << std::endl;

	// 0%, 25%, 50%, 75%, 100%
	uint64_t results[] = {0x16c09391c25947f7, 0x8bf70a7628ee2f30, 0x573d220db2cf2dce, 0x4418b7386d2917f1, 0x207d7d23fdc96763};

	bench.result(results[NumSequential], num_found);
}

static BenchRegister reg(RandomFind<0>, RandomFind<1>, RandomFind<2>, RandomFind<3>, RandomFind<4>);