#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

static const char* HashName = "Identity";

// Linux uses an identity hash that does nothing. It's obviously fast, but extemely bad hash. Some hashmap implementations can't deal at all with
// this.
template <typename T>
struct Hash {
    size_t operator()(T const& key) const {
        return static_cast<size_t>(key);
    }
};

// default hash
template <>
struct Hash<std::string> {
    size_t operator()(std::string const& key) const {
        return std::hash<std::string>{}(key);
    }
};