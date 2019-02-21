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
        verifier += map.erase(rng() & bitMask);
    }
    return verifier;
}

BENCHMARK(RandomInsertErase) {
    bench.endMeasure(50004640, run(100000000, UINT64_C(0x00000000'00000FFF), bench));
    bench.endMeasure(51606619, run(100000000, UINT64_C(0x00F00000'00000000), bench));
    bench.endMeasure(49984422, run(100000000, UINT64_C(0x000000FF'000000FF), bench));
    bench.endMeasure(50093276, run(100000000, UINT64_C(0x0000000F'0000000F), bench));
    bench.endMeasure(50096606, run(100000000, UINT64_C(0xF000000F'00000000), bench));
    bench.endMeasure(49987113, run(100000000, UINT64_C(0x0000FFFF'00000000), bench));
    bench.endMeasure(1558700, run(30000000, UINT64_C(0xF00F0F0F'F0F00F00), bench));
}
