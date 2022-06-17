#include "Map.h"
#include "bench.h"

BENCHMARK(CtorDtorEmptyMap) {
    size_t result = 0;
    bench.beginMeasure("ctor & dtor empty map");
    for (size_t n = 0; n < 100'000'000; ++n) {
        using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
        Resource<int, int> resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif
        result += map.size();
    }
    bench.endMeasure(0, result);
}

BENCHMARK(CtorDtorSingleEntryMap) {
    size_t result = 0;
    bench.beginMeasure("ctor & dtor map with 1 entry");
    for (int n = 0; n < 50'000'000; ++n) {
        using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
        Resource<int, int> resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif
        map[n];
        result += map.size();
    }
    bench.endMeasure(50'000'000, result);
}
