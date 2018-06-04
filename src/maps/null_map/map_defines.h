#pragma once

#include <cstddef>
#include <utility>

// dummy map that does nothing, for overhead estimation
const char* MapName = "dummy map";

template <class Key, class Val>
class Map {
public:
    typedef std::pair<const Key, Val> value_type;

    constexpr Val& operator[](const Key&) {
        return val;
    }

    constexpr size_t size() { return 0; }
    constexpr void erase(const Key&) {}

    constexpr value_type const* begin() const {
        return (value_type*)0;
    }
    
    constexpr value_type const* end() const {
        return (value_type*)0;
    }

private:
    Val val;
};
