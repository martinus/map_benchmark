#include "Map.h"
#include "RandomBool.h"
#include "bench.h"
#include "sfc64.h"

#include <algorithm>
#include <iomanip>

template <size_t NumRandom>
uint64_t RandomFind(Bench& bench) {
    size_t constexpr NumTotal = 4;
    size_t constexpr NumSequential = NumTotal - NumRandom;

    size_t constexpr NumInserts = 1000000;
    size_t constexpr NumFindsPerIter = 1000 * NumTotal;

    // just multiply numbers with a small factor so we don't just sequentially insert numbers
    size_t constexpr NotSequentialFactor = 31;

    std::stringstream ss;
    ss << "RandomFind " << std::setw(3) << (NumSequential * 100 / NumTotal) << "% find success";
    auto title = ss.str();

    sfc64 rng(123);

    size_t num_found = 0;

    std::array<bool, NumTotal> insertRandom = {false};
    for (size_t i = 0; i < NumRandom; ++i) {
        insertRandom[i] = true;
    }

    sfc64 anotherUnrelatedRng(987654321);
    auto const anotherUnrelatedRngInitialState = anotherUnrelatedRng.state();
    sfc64 findRng(anotherUnrelatedRngInitialState);

    {
        Map<size_t, size_t> map;
        size_t i = 0;
        size_t findCount = 0;

        bench.beginMeasure({title.c_str(), MapName, HashName});
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
    }
    return num_found;
}

BENCHMARK(RandomFind_0) {
    auto result = RandomFind<4>(bench);
    bench.endMeasure(113878, result);
}

BENCHMARK(RandomFind_25) {
    auto result = RandomFind<3>(bench);
    bench.endMeasure(250078938, result);
}

BENCHMARK(RandomFind_50) {
    auto result = RandomFind<2>(bench);
    bench.endMeasure(500041672, result);
}

BENCHMARK(RandomFind_75) {
    auto result = RandomFind<1>(bench);
    bench.endMeasure(750019000, result);
}

BENCHMARK(RandomFind_100) {
    auto result = RandomFind<0>(bench);
    bench.endMeasure(999987348, result);
}
