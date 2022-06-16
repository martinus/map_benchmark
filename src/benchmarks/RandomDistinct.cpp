#include "Map.h"
#include "bench.h"
#include "sfc64.h"

BENCHMARK(RandomDistinct2) {
    static size_t const n = 50'000'000;

    sfc64 rng(123);

    int checksum;
    {
        bench.beginMeasure("5% distinct");
        checksum = 0;
        size_t const max_rng = n / 20;

        using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
        M::allocator_type::ResourceType resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif
        for (size_t i = 0; i < n; ++i) {
            checksum += ++map[static_cast<int>(rng(max_rng))];
        }
    }
    bench.endMeasure(549985352, checksum);

    {
        bench.beginMeasure("25% distinct");
        checksum = 0;
        size_t const max_rng = n / 4;

        using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
        M::allocator_type::ResourceType resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif
        for (size_t i = 0; i < n; ++i) {
            checksum += ++map[static_cast<int>(rng(max_rng))];
        }
    }
    bench.endMeasure(149979034, checksum);

    {
        bench.beginMeasure("50% distinct");
        size_t const max_rng = n / 2;

        using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
        M::allocator_type::ResourceType resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif
        for (size_t i = 0; i < n; ++i) {
            checksum += ++map[static_cast<int>(rng(max_rng))];
        }
    }
    bench.endMeasure(249981806, checksum);

    {
        bench.beginMeasure("100% distinct");
        checksum = 0;
        using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
        M::allocator_type::ResourceType resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif
        for (size_t i = 0; i < n; ++i) {
            checksum += ++map[static_cast<int>(rng())];
        }
    }
    bench.endMeasure(50291811, checksum);
}