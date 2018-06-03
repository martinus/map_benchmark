#pragma once

#include <cstddef>

// dummy map that does nothing, for overhead estimation
const char* MapName = "dummy map";
const double MapScale = 20;


template <class Key, class Val>
class Map {
public:
    constexpr Val& operator[](const Key&) {
        return val;
    }

    constexpr size_t size() { return 0; }
    constexpr void erase(const Key&) {}

private:
    Val val;
};
