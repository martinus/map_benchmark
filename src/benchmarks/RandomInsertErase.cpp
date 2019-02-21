#include "Map.h"
#include "bench.h"
#include "sfc64.h"

#include <sstream>

size_t run(size_t max_n, uint64_t bitMask, Bench& bench) {
    sfc64 rng(123);

    // time measured part
    size_t verifier = 0;
    std::stringstream ss;
    ss << max_n << " @ 0x" << std::hex << bitMask << std::dec;

    bench.beginMeasure(ss.str().c_str());
    Map<uint64_t, uint64_t> map;
    for (size_t i = 0; i < max_n; ++i) {
        map[rng() & bitMask] = i;
        auto it = map.find(rng() & bitMask);
        if (it != map.end()) {
            verifier += it->second;
            map.erase(it);
        }
    }
    return verifier;
}

BENCHMARK(RandomInsertErase) {
    bench.endMeasure(2500192580072665, run(100000000, UINT64_C(0x00000000'00000FFF), bench));
    bench.endMeasure(2580309529973681, run(100000000, UINT64_C(0x00F00000'00000000), bench));
    bench.endMeasure(2498363333927799, run(100000000, UINT64_C(0x000000FF'000000FF), bench));
    bench.endMeasure(2504472903901015, run(100000000, UINT64_C(0x0000000F'0000000F), bench));
    bench.endMeasure(2504651273268435, run(100000000, UINT64_C(0xF000000F'00000000), bench));
    bench.endMeasure(2498608646258729, run(100000000, UINT64_C(0x0000FFFF'00000000), bench));
    bench.endMeasure(2466305693537, run(40000000, UINT64_C(0xF00F0F0F'F0F0FF00), bench));
}
