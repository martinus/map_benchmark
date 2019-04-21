#include "Map.h"
#include "bench.h"
#include "hex.h"
#include "sfc64.h"

#include <algorithm>
#include <iomanip>

uint64_t randomFindInternal(Bench& bench, size_t numRandom, uint64_t bitMask, size_t numInserts, size_t numFindsPerInsert) {
    size_t constexpr NumTotal = 4;
    size_t const numSequential = NumTotal - numRandom;

    size_t const numFindsPerIter = numFindsPerInsert * NumTotal;

    std::stringstream ss;
    ss << (numSequential * 100 / NumTotal) << "% success, " << hex(bitMask);
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
                    map[rng() & bitMask] = static_cast<size_t>(1);
                } else {
                    map[val & bitMask] = static_cast<size_t>(1);
                }
                ++i;
            }

            // the actual benchmark code which sohould be as fast as possible
            for (size_t j = 0; j < numFindsPerIter; ++j) {
                if (++findCount > i) {
                    findCount = 0;
                    findRng.state(anotherUnrelatedRngInitialState);
                }
                auto it = map.find(findRng() & bitMask);
                if (it != map.end()) {
                    num_found += it->second;
                }
            }
        } while (i < numInserts);
    }
    return num_found;
}

BENCHMARK(RandomFind_200) {
    static constexpr auto lower32bit = UINT64_C(0x00000000FFFFFFFF);
    static constexpr auto upper32bit = UINT64_C(0xFFFFFFFF00000000);
    static constexpr size_t numInserts = 200;
    static constexpr size_t numFindsPerInsert = 5'000'000;

    bench.endMeasure(0, randomFindInternal(bench, 4, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(0, randomFindInternal(bench, 4, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(244807024, randomFindInternal(bench, 3, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(244807024, randomFindInternal(bench, 3, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(489614048, randomFindInternal(bench, 2, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(489614048, randomFindInternal(bench, 2, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(734421072, randomFindInternal(bench, 1, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(734421072, randomFindInternal(bench, 1, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(979228096, randomFindInternal(bench, 0, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(979228096, randomFindInternal(bench, 0, upper32bit, numInserts, numFindsPerInsert));
}

BENCHMARK(RandomFind_2000) {
    static constexpr auto lower32bit = UINT64_C(0x00000000FFFFFFFF);
    static constexpr auto upper32bit = UINT64_C(0xFFFFFFFF00000000);
    static constexpr size_t numInserts = 2000;
    static constexpr size_t numFindsPerInsert = 500'000;

    bench.endMeasure(0, randomFindInternal(bench, 4, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(0, randomFindInternal(bench, 4, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(249194555, randomFindInternal(bench, 3, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(249194555, randomFindInternal(bench, 3, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(498389112, randomFindInternal(bench, 2, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(498389112, randomFindInternal(bench, 2, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(747583668, randomFindInternal(bench, 1, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(747583668, randomFindInternal(bench, 1, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(996778223, randomFindInternal(bench, 0, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(996778223, randomFindInternal(bench, 0, upper32bit, numInserts, numFindsPerInsert));
}

BENCHMARK(RandomFind_500000) {
    static constexpr auto lower32bit = UINT64_C(0x00000000FFFFFFFF);
    static constexpr auto upper32bit = UINT64_C(0xFFFFFFFF00000000);
    static constexpr size_t numInserts = 500'000;
    static constexpr size_t numFindsPerInsert = 1000;

    bench.endMeasure(33571, randomFindInternal(bench, 4, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(29114, randomFindInternal(bench, 4, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(125018398, randomFindInternal(bench, 3, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(125016517, randomFindInternal(bench, 3, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(250006412, randomFindInternal(bench, 2, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(250004852, randomFindInternal(bench, 2, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(374997827, randomFindInternal(bench, 1, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(374996290, randomFindInternal(bench, 1, upper32bit, numInserts, numFindsPerInsert));

    bench.endMeasure(499988041, randomFindInternal(bench, 0, lower32bit, numInserts, numFindsPerInsert));
    bench.endMeasure(499988041, randomFindInternal(bench, 0, upper32bit, numInserts, numFindsPerInsert));
}
