#include "Map.h"
#include "bench.h"
#include "sfc64.h"

#include <sstream>

template <int RngShift>
size_t run(size_t max_n, Bench& bench) {
    sfc64 rng(123);

    // time measured part
    size_t verifier = 0;
    std::stringstream ss;
    ss << "random insert & erase, leftshift " << RngShift << " bits";

    bench.beginMeasure({ss.str().c_str(), MapName, HashName});
    {
        Map<uint64_t, uint64_t> map;
        for (size_t n = 2; n < max_n; ++n) {
            for (size_t i = 0; i < max_n; ++i) {
                map[rng(n) << RngShift] = i;
                verifier += map.erase(rng(n) << RngShift);
            }
        }
    }
    return verifier;
}

BENCHMARK(RandomInsertErase) {
    auto result = run<0>(21000, bench);
    bench.endMeasure(321, result);
}
BENCHMARK(RandomInsertEraseShifted_4) {
    auto result = run<4>(21000, bench);
    bench.endMeasure(3321, result);
}
