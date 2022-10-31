#pragma once

#include "avaneev__komihash/komihash.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

static const char* HashName = "komihash";

template <typename Key, typename Enable = void>
struct Hash;

template <typename Key>
struct Hash<Key, typename std::enable_if_t<std::has_unique_object_representations_v<Key>>> {
    using is_avalanching = void;
    auto operator()(Key const& k) const noexcept -> uint64_t {
        return komihash(&k, sizeof(k), 0);
    }
};

template <typename CharT>
struct Hash<std::basic_string<CharT>> {
    using is_avalanching = void;
    auto operator()(std::basic_string_view<CharT> const& sv) const noexcept -> uint64_t {
        return komihash(sv.data(), sizeof(CharT) * sv.size(), 0);
    }
};

template <typename CharT>
struct Hash<std::basic_string_view<CharT>> {
    using is_avalanching = void;
    auto operator()(std::basic_string_view<CharT> const& sv) const noexcept -> uint64_t {
        return komihash(sv.data(), sizeof(CharT) * sv.size(), 0);
    }
};
