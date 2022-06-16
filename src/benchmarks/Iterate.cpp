#include "Map.h"
#include "bench.h"
#include "sfc64.h"

BENCHMARK(IterateIntegers) {
    sfc64 rng(123);

    size_t const num_iters = 50000;

    uint64_t result = 0;

    using M = Map<uint64_t, uint64_t>;
#ifdef USE_POOL_ALLOCATOR
    M::allocator_type::ResourceType resource;
    M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M map;
#endif

    auto const state = rng.state();
    bench.beginMeasure("iterate while adding");
    for (size_t n = 0; n < num_iters; ++n) {
        map[rng()] = n;
        for (auto const& keyVal : map) {
            result += keyVal.second;
        }
    }
    bench.endMeasure(UINT64_C(20833333325000), result);

    rng.state(state);
    bench.beginMeasure("iterate while removing");
    for (size_t n = 0; n < num_iters; ++n) {
        map.erase(rng());
        for (auto const& keyVal : map) {
            result += keyVal.second;
        }
    }
    bench.endMeasure(UINT64_C(62498750000000), result);
}
