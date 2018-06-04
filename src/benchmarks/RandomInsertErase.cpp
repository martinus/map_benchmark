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

BenchRegister reg(RandomInsertErase);