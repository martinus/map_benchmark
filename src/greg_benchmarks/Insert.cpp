#include "Map.h"
#include "bench.h"
#include "sfc64.h"

BENCHMARK(InsertManyInt) {
    sfc64 rng(213);

    {
        bench.beginMeasure("insert 100M int");
        Map<int, int> map;
        for (size_t n = 0; n < 100'000'000; ++n) {
            map[static_cast<int>(rng())] = 27;
        }
        bench.endMeasure(98841586, map.size());
    }
}
