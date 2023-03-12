#include "Map.h"
#include "bench.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class vec2 {
    uint32_t m_xy;

public:
    constexpr vec2(uint16_t x, uint16_t y)
        : m_xy{static_cast<uint32_t>(x) << 16U | y} {}

    constexpr explicit vec2(uint32_t xy)
        : m_xy(xy) {}

    [[nodiscard]] constexpr auto pack() const -> uint32_t {
        return m_xy;
    };

    [[nodiscard]] constexpr auto add_x(uint16_t x) const -> vec2 {
        return vec2{m_xy + (static_cast<uint32_t>(x) << 16U)};
    }

    [[nodiscard]] constexpr auto add_y(uint16_t y) const -> vec2 {
        return vec2{(m_xy & 0xffff0000) | ((m_xy + y) & 0xffff)};
    }

    template <typename Op>
    constexpr void for_each_surrounding(Op&& op) const {
        uint32_t v = m_xy;

        uint32_t upper = (v & 0xffff0000U);
        uint32_t l1 = (v - 1) & 0xffffU;
        uint32_t l2 = v & 0xffffU;
        uint32_t l3 = (v + 1) & 0xffffU;

        op((upper - 0x10000) | l1);
        op((upper - 0x10000) | l2);
        op((upper - 0x10000) | l3);

        op(upper | l1);
        // op(upper | l2);
        op(upper | l3);

        op((upper + 0x10000) | l1);
        op((upper + 0x10000) | l2);
        op((upper + 0x10000) | l3);
    }
};

template <typename M>
void game_of_life(Bench& bench, const char* name, size_t nsteps, size_t finalPopulation, M& map1, std::vector<vec2> state) {
    bench.beginMeasure(name);

    map1.clear();
    auto map2 = map1;

    for (auto& v : state) {
        v = v.add_x(UINT16_MAX / 2).add_y(UINT16_MAX / 2);
        map1[v.pack()] = true;
        v.for_each_surrounding([&](uint32_t xy) { map1.emplace(xy, false); });
    }

    auto* m1 = &map1;
    auto* m2 = &map2;
    for (size_t i = 0; i < nsteps; ++i) {
        for (auto const kv : *m1) {
            auto const& pos = kv.first;
            auto alive = kv.second;
            int neighbors = 0;
            vec2{pos}.for_each_surrounding([&](uint32_t xy) {
                if (auto x = m1->find(xy); x != m1->end()) {
                    neighbors += x->second;
                }
            });
            if ((alive && (neighbors == 2 || neighbors == 3)) || (!alive && neighbors == 3)) {
                (*m2)[pos] = true;
                vec2{pos}.for_each_surrounding([&](uint32_t xy) { m2->emplace(xy, false); });
            }
        }
        m1->clear();
        std::swap(m1, m2);
    }

    size_t count = 0;
    for (auto const kv : *m1) {
        count += kv.second;
    }
    bench.endMeasure(finalPopulation, count);
}

BENCHMARK(GameOfLife_stabilizing) {
    using M = Map<uint32_t, bool>;
#ifdef USE_POOL_ALLOCATOR
    Resource<uint32_t, bool> resource;
    M map1{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M map1;
#endif

    // https://conwaylife.com/wiki/R-pentomino
    game_of_life(bench, "R-pentomino", 1103, 116, map1, {{1, 0}, {2, 0}, {0, 1}, {1, 1}, {1, 2}});

    // https://conwaylife.com/wiki/Acorn
    game_of_life(bench, "Acorn", 5206, 633, map1, {{1, 0}, {3, 1}, {0, 2}, {1, 2}, {4, 2}, {5, 2}, {6, 2}});

    // https://conwaylife.com/wiki/Jaydot
    game_of_life(bench, "Jaydot", 6929, 1124, map1, {{1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 3}, {1, 4}, {2, 4}, {0, 5}});

    // https://conwaylife.com/wiki/Bunnies
    game_of_life(bench, "Bunnies", 17332, 1744, map1, {{0, 0}, {6, 0}, {2, 1}, {6, 1}, {2, 2}, {5, 2}, {7, 2}, {1, 3}, {3, 3}});
}

BENCHMARK(GameOfLife_growing) {
    using M = Map<uint32_t, bool>;
#ifdef USE_POOL_ALLOCATOR
    Resource<uint32_t, bool> resource;
    M map1{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M map1;
#endif

    // https://conwaylife.com/wiki/Gotts_dots
    game_of_life(bench, "Gotts dots", 2000, 4599, map1,
                 {
                     {0, 0},    {0, 1},    {0, 2},                                                                                 // 1
                     {4, 11},   {5, 12},   {6, 13},   {7, 12},   {8, 11},                                                          // 2
                     {9, 13},   {9, 14},   {9, 15},                                                                                // 3
                     {185, 24}, {186, 25}, {186, 26}, {186, 27}, {185, 27}, {184, 27}, {183, 27}, {182, 26},                       // 4
                     {179, 28}, {180, 29}, {181, 29}, {179, 30},                                                                   // 5
                     {182, 32}, {183, 31}, {184, 31}, {185, 31}, {186, 31}, {186, 32}, {186, 33}, {185, 34},                       // 6
                     {175, 35}, {176, 36}, {170, 37}, {176, 37}, {171, 38}, {172, 38}, {173, 38}, {174, 38}, {175, 38}, {176, 38}, // 7
                 });

    // https://conwaylife.com/wiki/Puffer_2
    game_of_life(bench, "Puffer 2", 2000, 7400, map1,
                 {
                     {1, 0}, {2, 0}, {3, 0},  {15, 0}, {16, 0}, {17, 0}, // line 0
                     {0, 1}, {3, 1}, {14, 1}, {17, 1},                   // line 1
                     {3, 2}, {8, 2}, {9, 2},  {10, 2}, {17, 2},          // line 2
                     {3, 3}, {8, 3}, {11, 3}, {17, 3},                   // line 3
                     {2, 4}, {7, 4}, {16, 4},                            // line 4
                 });
}
