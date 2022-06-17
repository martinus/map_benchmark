#include "Map.h"
#include "bench.h"
#include "hex.h"
#include "sfc64.h"
#include "shuffle.h"

#include <algorithm>
#include <iomanip>

uint64_t randomFindInternalString(Bench& bench, size_t numRandom, size_t const length, size_t numInserts, size_t numFindsPerInsert) {
    size_t constexpr NumTotal = 4;
    size_t const numSequential = NumTotal - numRandom;

    size_t const numFindsPerIter = numFindsPerInsert * NumTotal;

    std::stringstream ss;
    ss << (numSequential * 100 / NumTotal) << "% " << length << " byte";
    auto title = ss.str();

    sfc64 rng(123);

    size_t num_found = 0;

    std::array<bool, NumTotal> insertRandom = {false};
    for (size_t i = 0; i < numRandom; ++i) {
        insertRandom[i] = true;
    }

    sfc64 anotherUnrelatedRng(987654321);
    auto const anotherUnrelatedRngInitialState = anotherUnrelatedRng.state();
    sfc64 findRng(anotherUnrelatedRngInitialState);

    std::string str(length, 'y');
    // point to the back of the string (32bit aligned), so comparison takes a while
    auto const idx32 = (length / 4) - 1;
    auto const strData32 = reinterpret_cast<uint32_t*>(&str[0]) + idx32;

    {
        using M = Map<std::string, size_t>;
#ifdef USE_POOL_ALLOCATOR
        Resource<std::string, size_t> resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif
        size_t i = 0;
        size_t findCount = 0;

        bench.beginMeasure(title.c_str());
        do {

            // insert NumTotal entries: some random, some sequential.
            slightlyBiasedShuffle(insertRandom.begin(), insertRandom.end(), rng);
            for (bool isRandomToInsert : insertRandom) {
                auto val = anotherUnrelatedRng();
                if (isRandomToInsert) {
                    *strData32 = rng();
                } else {
                    *strData32 = val;
                }
                map[str] = static_cast<size_t>(1);
                ++i;
            }

            // the actual benchmark code which sohould be as fast as possible
            for (size_t j = 0; j < numFindsPerIter; ++j) {
                if (++findCount > i) {
                    findCount = 0;
                    findRng.state(anotherUnrelatedRngInitialState);
                }
                *strData32 = findRng();
                auto it = map.find(str);
                if (it != map.end()) {
                    num_found += it->second;
                }
            }
        } while (i < numInserts);
    }
    return num_found;
}

BENCHMARK(RandomFindString) {
    static constexpr size_t numInserts = 100'000;
    static constexpr size_t numFindsPerInsert = 1000;

    bench.endMeasure(1721, randomFindInternalString(bench, 4, 100, numInserts, numFindsPerInsert));
    bench.endMeasure(24999056, randomFindInternalString(bench, 3, 100, numInserts, numFindsPerInsert));
    bench.endMeasure(49996344, randomFindInternalString(bench, 2, 100, numInserts, numFindsPerInsert));
    bench.endMeasure(74992237, randomFindInternalString(bench, 1, 100, numInserts, numFindsPerInsert));
    bench.endMeasure(99989650, randomFindInternalString(bench, 0, 100, numInserts, numFindsPerInsert));
}

BENCHMARK(RandomFindString_1000000) {
    static constexpr size_t numInserts = 1'000'000;
    static constexpr size_t numFindsPerInsert = 200;

    bench.endMeasure(26187, randomFindInternalString(bench, 4, 13, numInserts, numFindsPerInsert));
    bench.endMeasure(50017770, randomFindInternalString(bench, 3, 13, numInserts, numFindsPerInsert));
    bench.endMeasure(100012105, randomFindInternalString(bench, 2, 13, numInserts, numFindsPerInsert));
    bench.endMeasure(150003241, randomFindInternalString(bench, 1, 13, numInserts, numFindsPerInsert));
    bench.endMeasure(199997474, randomFindInternalString(bench, 0, 13, numInserts, numFindsPerInsert));
}
