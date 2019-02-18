#include "bench.h"

#include "Map.h"

BENCHMARK(CtorDtorEmptyMap) {
    size_t result = 0;
    bench.beginMeasure("ctor & dtor empty map");
    for (size_t n = 0; n < 100'000'000; ++n) {
        Map<int, int> map;
        result += map.size();
    }
    bench.endMeasure(0, result);
}

BENCHMARK(CtorDtorSingleEntryMap) {
    size_t result = 0;
    bench.beginMeasure("ctor & dtor map with 1 entry");
    for (size_t n = 0; n < 50'000'000; ++n) {
        Map<int, int> map;
        map[123];
        result += map.size();
    }
    bench.endMeasure(50'000'000, result);
}
