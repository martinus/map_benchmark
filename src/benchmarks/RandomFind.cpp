#include "RandomBool.h"
#include "bench.h"

#include <iomanip>

template <size_t NumRandom>
static void RandomFind(Bench& bench) {
	size_t constexpr NumTotal = 4;
	size_t constexpr NumSequential = NumTotal - NumRandom;

	size_t constexpr SpreadFactor = 55967; // a random prime

	size_t constexpr NumInserts = 1000000;
	size_t constexpr NumFindsPerIter = 1000 * NumTotal;

	std::stringstream ss;
	ss << "RandomFind " << std::setw(3) << (NumSequential * 100 / NumTotal) << "% find success";
	bench.title(ss.str());
	auto& rng = bench.rng();
	RandomBool rbool;

	size_t num_found = 0;

	bench.beginMeasure();
	{
		Map<size_t, size_t> map;
		size_t i = 0;
		do {
			for (size_t j = 0; j < std::max(NumSequential, NumRandom); ++j) {
				if (j < NumSequential) {
					map.emplace(i, i);
					++i;
				}
				if (j < NumRandom) {
					map.emplace(static_cast<size_t>(rng.uniform<size_t>()), i);
					++i;
				}
			}

			for (size_t j = 0; j < NumFindsPerIter; ++j) {
				num_found += map.count(static_cast<size_t>(rng.uniform<size_t>(i)));
			}
		} while (i < NumInserts);
		bench.event("done");
	}
	bench.event("dtor");
	bench.endMeasure();

	// 0%, 25%, 50%, 75%, 100%
	uint64_t results[] = {0x2d54a7bfc54082e7, 0x3b59d87352fd7294, 0x254acfc7749b7405, 0xd10322020f5c0ceb, 0xf9835cc734661c2b};

	bench.result(results[NumSequential], num_found);
}

static BenchRegister reg(RandomFind<0>, RandomFind<1>, RandomFind<2>, RandomFind<3>, RandomFind<4>);