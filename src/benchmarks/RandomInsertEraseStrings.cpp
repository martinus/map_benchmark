#include "Map.h"
#include "bench.h"
#include "sfc64.h"

#include <sstream>

size_t run(size_t max_n, size_t string_length, uint32_t bitMask, Bench& bench) {
    sfc64 rng(123);

    // time measured part
    size_t verifier = 0;
    std::stringstream ss;
    ss << string_length << " bytes" << std::dec;

    std::string str(string_length, 'x');
    auto strData32 = reinterpret_cast<uint32_t*>(&str[0]);

    bench.beginMeasure(ss.str().c_str());
    Map<std::string, std::string> map;
    for (size_t i = 0; i < max_n; ++i) {
        *strData32 = rng() & bitMask;

        // create an entry.
        map[str];

        *strData32 = rng() & bitMask;
        auto it = map.find(str);
        if (it != map.end()) {
            ++verifier;
            map.erase(it);
        }
    }
    return verifier;
}

BENCHMARK(RandomInsertEraseStrings) {
    bench.endMeasure(9734165, run(20000000, 7, 0xfffff, bench));
    bench.endMeasure(9734165, run(20000000, 8, 0xfffff, bench));
    bench.endMeasure(9734165, run(20000000, 13, 0xfffff, bench));
    bench.endMeasure(5867228, run(12000000, 100, 0x7ffff, bench));
    bench.endMeasure(2966452, run(6000000, 1000, 0x1ffff, bench));
}