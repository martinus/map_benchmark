#pragma once

#include <functional>
#include <iostream>
#include <type_traits>

// final step from MurmurHash
inline uint64_t fmix64(uint64_t k) {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdULL;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ULL;
    k ^= k >> 33;
    return k;
}

// from boost::hash_combine, with additional fmix
inline void hash_combine(uint64_t& seed, uint64_t value) {
    seed ^= fmix64(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class T>
inline uint64_t hash_value(const T& value) {
    return std::hash<typename std::decay<decltype(value)>::type>{}(value);
}

// calculates a hash of any iterable map. Order is irrelevant.
template <class M>
inline uint64_t mapHash(const M& map) {
    uint64_t h = 1;

    size_t numElements = 0;
    for (auto const& entry : map) {
        uint64_t seed = 0;
        hash_combine(seed, hash_value(entry.first));
        hash_combine(seed, hash_value(entry.second));

        if (seed == 0) {
            seed = 0x1b3;
        }

        h *= seed;
        ++numElements;
    }

    hash_combine(h, numElements);
    return h;
}