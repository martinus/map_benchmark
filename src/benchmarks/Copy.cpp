#include "Map.h"
#include "bench.h"
#include "sfc64.h"
#include <cstdint>

BENCHMARK(Copy) {
    size_t result = 0;

    sfc64 rng(987);

    using M = Map<uint64_t, uint64_t>;
#ifdef USE_POOL_ALLOCATOR
    Resource<uint64_t, uint64_t> resource;
    M mapSource{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M mapSource;
#endif

    for (size_t i=0; i<1'000'000; ++i) {
        mapSource[i] = i;
    }

    M mapForCopy = mapSource;
    bench.beginMeasure("copy ctor");
    for (size_t n = 0; n < 200; ++n) {
        M m = mapForCopy;
        result += m.size() + m[static_cast<size_t>(500'000)];
        mapForCopy[rng()] = rng();
    }
    bench.endMeasure(300019900, result);

    mapForCopy = mapSource;
    bench.beginMeasure("copy assignment");
#ifdef USE_POOL_ALLOCATOR
    M m{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M m;
#endif
    for (size_t n = 0; n < 200; ++n) {
        m = mapForCopy;
        result += m.size() + m[static_cast<size_t>(500'000)];
        mapForCopy[rng()] = rng();
    }
    bench.endMeasure(600039800, result);
}
