#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

static const char* HashName = "FNV1a";

static size_t fnv1a(uint8_t const* data, size_t size) {
#if SIZE_MAX == UINT32_MAX
    static constexpr size_t FNV_offset_basis = UINT64_C(14695981039346656037);
    static constexpr size_t FNV_prime = UINT64_C(1099511628211);
#else
    static constexpr size_t FNV_offset_basis = UINT32_C(2166136261);
    static constexpr size_t FNV_prime = UINT32_C(16777619);
#endif
    size_t val = FNV_offset_basis;
    for (size_t i = 0; i < size; ++i) {
        val ^= static_cast<size_t>(data[i]);
        val *= FNV_prime;
    }
    return val;
}

// Visual Studio uses a FNV1a implementation.
template <typename T>
struct Hash {
    size_t operator()(T const& key) const {
        return fnv1a(reinterpret_cast<uint8_t const*>(&key), sizeof(T));
    }
};

// Visual Studio uses a FNV1a implementation.
template <>
struct Hash<std::string> {
    size_t operator()(std::string const& key) const {
        return fnv1a(reinterpret_cast<uint8_t const*>(key.data()), key.size());
    }
};