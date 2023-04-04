#pragma once

#include "pdimov__mulpx_hash/mulxp_hash.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

static const char* HashName = "mulpx0";

template <typename Key, typename Enable = void>
struct Hash;

template <typename Key>
struct Hash<Key, typename std::enable_if_t<std::is_integral_v<Key>>> {
    using is_avalanching = void;
    inline constexpr auto operator()(Key const& k) const noexcept -> uint64_t {
        return mulxp0_hash(reinterpret_cast<unsigned char const*>(&k), sizeof(k), 0);
    }
};

template <typename CharT>
struct Hash<std::basic_string<CharT>> {
    using is_avalanching = void;
    inline constexpr auto operator()(std::basic_string<CharT> const& s) const noexcept -> uint64_t {
        return mulxp0_hash(reinterpret_cast<unsigned char const*>(s.data()), sizeof(CharT) * s.size(), 0);
    }
};

template <typename CharT>
struct Hash<std::basic_string_view<CharT>> {
    using is_avalanching = void;
    inline constexpr auto operator()(std::basic_string_view<CharT> const sv) const noexcept -> uint64_t {
        return mulxp0_hash(reinterpret_cast<unsigned char const*>(sv.data()), sizeof(CharT) * sv.size(), 0);
    }
};
