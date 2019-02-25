#pragma once

#include <iomanip>
#include <iostream>
#include <limits>

template <typename T>
struct ValAsHex {
    T value;
};

template <typename T>
ValAsHex<T> hex(T val) {
    return ValAsHex<T>{val};
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, ValAsHex<T> const& h) {
    auto const f = os.flags();
    os << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << h.value;
    os.flags(f);
    return os;
}