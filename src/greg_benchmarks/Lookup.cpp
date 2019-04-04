#include "Map.h"
#include "bench.h"
#include "sfc64.h"

BENCHMARK(Lookup) {
    sfc64 rng(213);

    {
        auto const state = rng.state();
        Map<int, int> map;
        for (size_t n = 0; n < 1'000'000; ++n) {
            map[static_cast<int>(rng())] = 1;
        }

        size_t res = 0;
        auto end = map.end();
        bench.beginMeasure("Lookup 100m ints, all present");
        for (size_t i=0; i<100, ++i)
        {
            rng.state(state);
            for (size_t n = 0; n < 1'000'000; ++n) {
                auto it = map.find(static_cast<int>(rng()));
                if (it != end)
                    res += *it;
            }
        }
        bench.endMeasure(res, res);

        res = 0;
        bench.beginMeasure("Lookup 100m ints, few present");
        for (size_t i=0; i<100, ++i)
        {
            for (size_t n = 0; n < 1'000'000; ++n) {
                auto it = map.find(static_cast<int>(rng()));
                if (it != end)
                    res += *it;
            }
        }
        bench.endMeasure(res, res);
    }
}
