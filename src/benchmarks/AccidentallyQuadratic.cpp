#include "Map.h"
#include "bench.h"
#include "sfc64.h"

// see https://www.tumblr.com/accidentallyquadratic/153545455987/rust-hash-iteration-reinsertion
BENCHMARK(AccidentallyQuadratic) {
    sfc64 rng(12345);

    using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
    Resource<int, int> resource;
    M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M map;
#endif
    bench.beginMeasure("insert 10M int");
    for (size_t n = 0; n < 10'000'000; ++n) {
        map[static_cast<int>(rng())];
    }
    bench.endMeasure(9988513, map.size());

#if 1
    bench.beginMeasure("iterate");
    uint64_t sum = 0;
    for (auto const& kv : map) {
        sum += kv.first;
        sum += kv.second;
    }
    bench.endMeasure(UINT64_C(18446739465311920326), sum);

    bench.beginMeasure("iterate & copy");
#ifdef USE_POOL_ALLOCATOR
    M map2{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M map2;
#endif
    for (auto const& kv : map) {
        map2[kv.first] = kv.second;
    }
    bench.endMeasure(map.size(), map2.size());
#endif
}
