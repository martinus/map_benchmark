#pragma once

#include <cstdint>

namespace ankerl::mixer {

// 128bit multiplication of a*b, returning low 64bits, and sets high 64bits
// Source: https://github.com/martinus/robin-hood-hashing
//
// might be worth looking what MUM hash does:
// https://github.com/vnmakarov/mum-hash/blob/master/mum.h#L103
inline uint64_t umul128(uint64_t a, uint64_t b, uint64_t* high) noexcept {
#if defined(__SIZEOF_INT128__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    using uint128_t = unsigned __int128;
#pragma GCC diagnostic pop
#endif

    auto result = static_cast<uint128_t>(a) * static_cast<uint128_t>(b);
    *high = static_cast<uint64_t>(result >> 64U);
    return static_cast<uint64_t>(result);

#elif (defined(_MSC_VER) && SIZE_MAX == UINT64_MAX)
#include <intrin.h> // for __umulh
#pragma intrinsic(__umulh)
#ifndef _M_ARM64
#pragma intrinsic(_umul128)
#endif
#ifdef _M_ARM64
    *high = __umulh(a, b);
    return ((uint64_t)(a)) * (b);
#else
    return _umul128(a, b, high);
#endif
#else
#error No hardware umul
#endif
}

// 128bit multiply a and b, xor high and low result
inline uint64_t mumx(uint64_t a, uint64_t b) {
    uint64_t h;
    uint64_t l = umul128(a, b, &h);
    return h ^ l;
}

// 128bit multiply a and b, add high and low result
inline uint64_t muma(uint64_t a, uint64_t b) {
    uint64_t h;
    uint64_t l = umul128(a, b, &h);
    return h + l;
}

inline uint64_t mumxmumxx1(uint64_t v) {
    static constexpr auto a = UINT64_C(0x2ca7aea0ebd71d49);
    static constexpr auto b = UINT64_C(0x9e49b5a3555f2295);

    return mumx(mumx(v, a), v ^ b);
}

} // namespace ankerl::mixer
