#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

template <typename Rng>
uint32_t boundedSlightlyBiased(uint32_t range, Rng& rng) noexcept {
    static_assert(Rng::max() == std::numeric_limits<uint64_t>::max());

    uint64_t r32 = static_cast<uint32_t>(rng());
    auto multiResult = r32 * range;
    return static_cast<uint32_t>(multiResult >> 32U);
}

template <typename It, typename Rng>
void slightlyBiasedShuffle(It first, It last, Rng& rng) {
    size_t n = last - first;
    size_t i;
    for (i = n - 1; i > 0; --i) {
        using std::swap;
        swap(first[i], first[boundedSlightlyBiased(i + 1, rng)]);
    }
}
