#pragma once

#include <cstddef>
#include <random>

// Fast uniform random bool
// see https://www.reddit.com/r/cpp/comments/a4jh9b/fast_uniform_random_bools/
class RandomBool {
public:
    template <typename Rng>
    bool operator()(Rng& rng) {
        if (1 == m_rand) {
            m_rand = std::uniform_int_distribution<size_t>{}(rng) | s_mask_left1;
        }
        bool const ret = m_rand & 1;
        m_rand >>= 1;
        return ret;
    }

private:
    static constexpr const size_t s_mask_left1 = size_t(1) << (sizeof(size_t) * 8 - 1);
    size_t m_rand = 1;
};