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
    // point to the back of the string (32bit aligned), so comparison takes a while
    auto const idx32 = (string_length / 4) - 1;
    auto const strData32 = reinterpret_cast<uint32_t*>(&str[0]) + idx32;

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

size_t benchString(size_t max_n, size_t string_length, Bench& bench) {
    std::stringstream ss;
    ss << string_length << " bytes" << std::dec;

    std::string str(string_length, 'x');
    // point to the back of the string (32bit aligned), so comparison takes a while
    auto const strData64 = reinterpret_cast<uint64_t*>(&str[0]);

    Hash<std::string> hasher;
    bench.beginMeasure(ss.str().c_str());

    *strData64 = 1;
    auto begin = std::chrono::high_resolution_clock::now();
    size_t verifier = 0;
    for (size_t i = 0; i < max_n; ++i) {
        (*strData64) *= 1171;
        verifier += hasher(str);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto nanos = (std::chrono::duration<double>(end - begin).count() * 1e9 / max_n);

    std::cout.precision(2);
    std::cout << std::fixed << nanos << std::endl;
    return verifier;
}

/*
BENCHMARK(StringHashBench) {
    bench.endMeasure(0, benchString(300'000'000, 7, bench));
    bench.endMeasure(0, benchString(100'000'000, 8, bench));
    bench.endMeasure(0, benchString(200'000'000, 13, bench));
    bench.endMeasure(0, benchString(80'000'000, 100, bench));
    bench.endMeasure(0, benchString(10'000'000, 1000, bench));
}
*/
BENCHMARK(RandomInsertEraseStrings) {
    bench.endMeasure(9734165, run(20000000, 7, 0xfffff, bench));
    bench.endMeasure(9734165, run(20000000, 8, 0xfffff, bench));
    bench.endMeasure(9734165, run(20000000, 13, 0xfffff, bench));
    bench.endMeasure(5867228, run(12000000, 100, 0x7ffff, bench));
    bench.endMeasure(2966452, run(6000000, 1000, 0x1ffff, bench));
}