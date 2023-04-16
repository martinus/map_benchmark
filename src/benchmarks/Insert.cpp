#include "Map.h"
#include "bench.h"
#include "sfc64.h"

BENCHMARK(InsertHugeInt) {
    sfc64 rng(213);

    {
        bench.beginMeasure("insert 100M int");
        using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
        Resource<int, int> resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif
        for (size_t n = 0; n < 100'000'000; ++n) {
            map[static_cast<int>(rng())];
        }
        bench.endMeasure(98841586, map.size());

        bench.beginMeasure("clear 100M int");
        map.clear();
        bench.endMeasure(0, map.size());

        // remember the rng's state so we can remove like we've added
        auto const state = rng.state();
        bench.beginMeasure("reinsert 100M int");
        for (size_t n = 0; n < 100'000'000; ++n) {
            map[static_cast<int>(rng())];
        }
        bench.endMeasure(98843646, map.size());

        rng.state(state);
        bench.beginMeasure("remove 100M int");
        for (size_t n = 0; n < 100'000'000; ++n) {
            map.erase(static_cast<int>(rng()));
        }
        bench.endMeasure(0, map.size());

        bench.beginMeasure("destructor empty map");
    }
    bench.endMeasure(0, 0);
}

std::array<size_t, 7> counts = {
	200, 2000, 2000, 20000, 200000, 2000000, 20000000
};

std::array<size_t, 7> results = {
	20000000, 19999999, 19999998, 19999957, 19999529, 19995213, 19953523
};

BENCHMARK(CreateInsert) {
    sfc64 rng(213);
    for (size_t i = 0; i < counts.size(); ++i) {
        size_t count = counts[i];
        size_t repeats = counts.back() / count;
        size_t res = 0;

        bench.beginMeasure(("creating and inserting " + std::to_string(count) +
                            " ints " + std::to_string(repeats) +
                            " times").c_str());

        for (size_t j = 0; j < repeats; ++j) {

            using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
            Resource<int, int> resource;
            M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
            M map;
#endif

            for (size_t n = 0; n < count; ++n)
                map[static_cast<int>(rng())];
            res += map.size();
        }
        bench.endMeasure(results[i], res);
    }
}

BENCHMARK(ClearInsert) {
    sfc64 rng(213);
    for (size_t i = 0; i < counts.size(); ++i) {
        size_t count = counts[i];
        size_t repeats = counts.back() / count;
        size_t res = 0;

        using M = Map<int, int>;
#ifdef USE_POOL_ALLOCATOR
        Resource<int, int> resource;
        M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
        M map;
#endif

        bench.beginMeasure(("inserting and clearing " + std::to_string(count) +
                            " ints " + std::to_string(repeats) +
                            " times").c_str());

        for (size_t j = 0; j < repeats; ++j) {
            for (size_t n = 0; n < count; ++n)
                map[static_cast<int>(rng())];
            res += map.size();
            map.clear();
        }
        bench.endMeasure(results[i], res);
    }
}

