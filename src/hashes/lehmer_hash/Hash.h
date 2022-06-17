#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

static const char* HashName = "Lehmer";

// Super fast and super simple, Lehmer's generator.
// from https://lemire.me/blog/2019/03/19/the-fastest-conventional-random-number-generator-that-can-pass-big-crush/
template <typename T>
struct Hash {
    size_t operator()(T const& key) const {
        return (__uint128_t(key) * 0xda942042e4dd58b5) >> 64U;
    }
};

// default hash
template <>
struct Hash<std::string> {
    size_t operator()(std::string const& key) const {
        return std::hash<std::string>{}(key);
    }
};