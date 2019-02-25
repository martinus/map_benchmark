#include "Map.h"
#include "bench.h"
#include "hex.h"
#include "sfc64.h"

#include <algorithm>
#include <iomanip>

template <size_t NumRandom, uint64_t BitMask>
uint64_t RandomFindInternal(Bench& bench) {
    size_t constexpr NumTotal = 4;
    size_t constexpr NumSequential = NumTotal - NumRandom;

    size_t constexpr NumInserts = 1000000;
    size_t constexpr NumFindsPerIter = 1000 * NumTotal;

    // just multiply numbers with a small factor so we don't just sequentially insert numbers
    size_t constexpr NotSequentialFactor = 31;

    std::stringstream ss;
    ss << std::setw(3) << (NumSequential * 100 / NumTotal) << "% success, " << hex(BitMask);
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

        bench.beginMeasure(title.c_str());
        do {
            // insert NumTotal entries: some random, some sequential.
            std::shuffle(insertRandom.begin(), insertRandom.end(), rng);
            for (bool isRandomToInsert : insertRandom) {
                auto val = anotherUnrelatedRng();
                if (isRandomToInsert) {
                    map.emplace(rng() & BitMask, static_cast<size_t>(1));
                } else {
                    map.emplace(val & BitMask, static_cast<size_t>(1));
                }
                ++i;
            }

            for (size_t j = 0; j < NumFindsPerIter; ++j) {
                if (++findCount > i) {
                    findCount = 0;
                    findRng.state(anotherUnrelatedRngInitialState);
                }
                auto it = map.find(findRng() & BitMask);
                if (it != map.end()) {
                    num_found += it->second;
                }
            }
        } while (i < NumInserts);
    }
    return num_found;
}

BENCHMARK(RandomFind) {
    static constexpr auto lower32bit = UINT64_C(0x00000000FFFFFFFF);
    static constexpr auto upper32bit = UINT64_C(0xFFFFFFFF00000000);

    bench.endMeasure(125989, RandomFindInternal<4, lower32bit>(bench));
    bench.endMeasure(113878, RandomFindInternal<4, upper32bit>(bench));

    bench.endMeasure(250096760, RandomFindInternal<3, lower32bit>(bench));
    bench.endMeasure(250078938, RandomFindInternal<3, upper32bit>(bench));

    bench.endMeasure(500051318, RandomFindInternal<2, lower32bit>(bench));
    bench.endMeasure(500041672, RandomFindInternal<2, upper32bit>(bench));

    bench.endMeasure(750019230, RandomFindInternal<1, lower32bit>(bench));
    bench.endMeasure(750019000, RandomFindInternal<1, upper32bit>(bench));

    bench.endMeasure(999987348, RandomFindInternal<0, lower32bit>(bench));
    bench.endMeasure(999987348, RandomFindInternal<0, upper32bit>(bench));
}
