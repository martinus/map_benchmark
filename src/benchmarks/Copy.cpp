#include "Map.h"
#include "bench.h"
#include <cstdint>

BENCHMARK(Copy) {
    size_t result = 0;
    bench.beginMeasure("copy map");

    sfc64 rng(987);

    using M = Map<uint64_t, uint64_t>;
#ifdef USE_POOL_ALLOCATOR
    Resource<int, int> resource;
    M mapSource{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M mapSource;
#endif

    for (size_t i=0; i<100'000; ++i) {
        mapSource[rng()] = i;
    }

    M mapForCopy = mapSource;
    bench.beginMeasure("copy ctor");
    for (size_t n = 0; n < 100; ++n) {
        M m = mapForCopy;
        result += m.size();
        mapForCopy[rng()] = rng();
    }
    bench.endMeasure(0, result);

    mapForCopy = mapSource;
    bench.beginMeasure("copy assignment");
#ifdef USE_POOL_ALLOCATOR
    M m{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M m;
#endif
    for (size_t n = 0; n < 100; ++n) {
        m = mapForCopy;
        result += m.size();
        mapForCopy[rng()] = rng();
    }
    bench.endMeasure(0, result);
}
