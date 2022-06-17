#pragma once

#include "mixer.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

static const char* HashName = "mumx";

#include <cstdint>

// very fast high quality mixer. From https://github.com/martinus/better-faster-stronger-mixer
template <typename T>
struct Hash {
    size_t operator()(T const& v) const {
        static constexpr auto a = UINT64_C(0x2ca7aea0ebd71d49);
        return ankerl::mixer::mumx(v, a);
    }
};

// default hash
template <>
struct Hash<std::string> {
    size_t operator()(std::string const& key) const {
        return std::hash<std::string>{}(key);
    }
};
