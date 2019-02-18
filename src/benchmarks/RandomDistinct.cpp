#include "Map.h"
#include "bench.h"
#include "sfc64.h"

BENCHMARK(RandomDistinct2) {
    static size_t const n = 50'000'000;

    sfc64 rng(123);

    int checksum;
    {
        bench.beginMeasure({"5% distinct", MapName, HashName});
        checksum = 0;
        size_t const max_rng = n / 20;
        Map<int, int> map;
        for (size_t i = 0; i < n; ++i) {
            checksum += ++map[static_cast<int>(rng(max_rng))];
        }
    }
    bench.endMeasure(432423, checksum);

    {
        bench.beginMeasure({"25% distinct", MapName, HashName});
        checksum = 0;
        size_t const max_rng = n / 4;
        Map<int, int> map;
        for (size_t i = 0; i < n; ++i) {
            checksum += ++map[static_cast<int>(rng(max_rng))];
        }
    }
    bench.endMeasure(432423, checksum);

    {
        bench.beginMeasure({"50% distinct", MapName, HashName});
        size_t const max_rng = n / 2;
        Map<int, int> map;
        for (size_t i = 0; i < n; ++i) {
            checksum += ++map[static_cast<int>(rng(max_rng))];
        }
    }
    bench.endMeasure(432423, checksum);

    {
        bench.beginMeasure({"100% distinct", MapName, HashName});
        checksum = 0;
        Map<int, int> map;
        for (size_t i = 0; i < n; ++i) {
            checksum += ++map[static_cast<int>(rng())];
        }
    }
    bench.endMeasure(423432, checksum);
}