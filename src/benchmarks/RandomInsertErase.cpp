#include "map_defines.h"

#include "bench.h"

static void RandomInsertErase(Bench& bench) {
    bench.title("RandomInsertErase: 10'000'000 random [] and erase of 10000 different values");

    // setup
    Map<int, int> map;
    const size_t iters = 10'000'000;

    // time measured part
    bench.beginMeasure();
    for (size_t i=0; i<iters; ++i) {
        map[bench.rng()(10'000)] = i;
        map.erase(bench.rng()(10'000));
    }
    bench.endMeasure();

    // result map status
    bench.result(mapHash(map));
}

static void RandomInsertEraseStrings(Bench& bench) {
    bench.title("RandomInsertEraseStrings: map with strings");
    
    // setup
    Map<std::string, int> map;
    const size_t iters = 1'000'000;

    std::string str(100, 'x');

    // time measured part
    bench.beginMeasure();
    for (size_t i=0; i<iters; ++i) {
        *reinterpret_cast<uint64_t*>(&str[20]) = bench.rng()(10000);
        map[str] = i;
        *reinterpret_cast<uint64_t*>(&str[20]) = bench.rng()(10000);        
        map.erase(str);
    }
    bench.endMeasure();

    // result map status
    bench.result(mapHash(map));
}

BenchRegister reg(RandomInsertErase, RandomInsertEraseStrings);