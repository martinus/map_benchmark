#include "Map.h"
#include "bench.h"

#include <vector>

struct Vec2 {
    int16_t x, y;
    Vec2(int16_t x, int16_t y) : x(x), y(y) {}
    inline uint32_t pack() const { return (((uint32_t)x) << 16) | (uint16_t)y; };
    static inline Vec2 unpack(uint32_t p) { return Vec2((int16_t)(p >> 16), (int16_t)p); }
};

void game_of_life(
        Bench &bench,
        const char *name,
        size_t nsteps,
        size_t finalPopulation,
        std::vector<Vec2> state) {
    bench.beginMeasure(name);
    Vec2 it(0,0);

    using M = Map<uint32_t, bool>;
#ifdef USE_POOL_ALLOCATOR
    Resource<uint32_t, bool> resource;
    M map1{0, M::hasher{}, M::key_equal{}, &resource};
    M map2{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M map1;
    M map2;
#endif

    for (auto const& v : state) {
        map1[v.pack()] = true;
        for (it.x = v.x - 1; it.x <= v.x + 1; ++it.x)
            for (it.y = v.y - 1; it.y <= v.y + 1; ++it.y)
                if (it.pack() != v.pack()) map1.emplace(it.pack(), false);
    }

    for (size_t i = 0; i < nsteps; ++i) {
        M &m1 = i&1 ? map2 : map1,
                        &m2 = i&1 ? map1 : map2;

        for (auto kv : m1) {
            Vec2 pos = Vec2::unpack(kv.first);
            bool alive = kv.second;
            int neighbors = 0;
            for (it.x = pos.x - 1; it.x <= pos.x + 1; ++it.x) {
                for (it.y = pos.y - 1; it.y <= pos.y + 1; ++it.y) {
                    if (it.pack() == pos.pack()) continue;
                    auto x = m1.find(it.pack());
                    if (x != m1.end())
                        neighbors += x->second;
                }
            }
            if ((alive && (neighbors == 2 || neighbors == 3)) || (!alive && neighbors == 3)) {
                m2[pos.pack()] = true;
                for (it.x = pos.x - 1; it.x <= pos.x + 1; ++it.x)
                    for (it.y = pos.y - 1; it.y <= pos.y + 1; ++it.y)
                        if (it.pack() != pos.pack()) m2.emplace(it.pack(), false);
            }
        }
        m1.clear();
    }

    size_t count = 0;
    M &m = nsteps&1 ? map2 : map1;
    for (auto kv : m) { count += kv.second; }
    bench.endMeasure(finalPopulation, count);
}


BENCHMARK(GameOfLife_stabilizing) {
    // https://conwaylife.com/wiki/R-pentomino
    game_of_life(bench, "R-pentomino", 1103, 116, {
        {1,0}, {2,0}, {0,1}, {1,1}, {1,2},
    });

    // https://conwaylife.com/wiki/Acorn
    game_of_life(bench, "Acorn", 5206, 633, {
        {1,0}, {3,1}, {0,2}, {1,2}, {4,2}, {5,2}, {6,2}
    });

    // https://conwaylife.com/wiki/Jaydot
    game_of_life(bench, "Jaydot", 6929, 1124, {
        {1,0}, {2,0}, {0,1}, {1,1}, {2,1}, {1,3}, {1,4}, {2,4}, {0,5}
    });

    // https://conwaylife.com/wiki/Bunnies
    game_of_life(bench, "Bunnies", 17332, 1744, {
        {0,0}, {6,0}, {2,1}, {6,1}, {2,2}, {5,2}, {7,2}, {1,3}, {3,3}
    });
}


BENCHMARK(GameOfLife_growing) {
    // https://conwaylife.com/wiki/Gotts_dots
    game_of_life(bench, "Gotts dots", 2000, 4091, {
        {0,0}, {0,1}, {0,2},
        {4,11}, {5,12}, {6,13}, {7,12}, {8,11},
        {9,13}, {9,14}, {9,14},
        {185,24}, {186,25}, {186,26}, {186,27}, {185,27}, {184,27}, {183,27}, {182,26},
        {179,28}, {180,29}, {181,29}, {179,30},
        {182,32}, {183,31}, {184,31}, {185,31}, {186,31}, {186,32}, {186,33}, {185,34},
        {175,35}, {176,36}, {170,37}, {176,37}, {171,38}, {172,38}, {173,38}, {174,38}, {175,38}, {176,38}
    });

    // https://conwaylife.com/wiki/Puffer_2
    game_of_life(bench, "Puffer 2", 2000, 7400, {
        {1,0}, {2,0}, {3,0}, {15,0}, {16,0}, {17,0},
        {0,1}, {3,1}, {14,1}, {17,1},
        {3,2}, {8,2}, {9,2}, {10,2}, {17,2},
        {3,3}, {8,3}, {11,3}, {17,3},
        {2,4}, {7,4}, {16,4}
    });
}

