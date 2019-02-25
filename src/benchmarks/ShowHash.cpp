#include "Hash.h"
#include "bench.h"

#include <bitset>
#include <iomanip>
#include <iostream>
#include <limits>

template <typename T>
struct HexRaii {
    explicit HexRaii(T val)
        : mVal(val) {}
    T mVal;
};

template <typename T>
HexRaii<T> hex(T val) {
    return HexRaii<T>(val);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, HexRaii<T> const& h) {
    auto const f = os.flags();
    os << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << h.mVal;
    os.flags(f);
    return os;
}

template <typename T>
void showHash(T val) {
    auto sh = std::hash<T>{}(val);
    auto mh = Hash<T>{}(val);
    std::cout << hex(val) << " -> " << hex(sh) << " " << hex(mh) << " " << std::bitset<sizeof(size_t) * 8>(mh) << std::endl;
}

template <typename T>
void showHash(const char* title) {
    std::cout << std::endl << title << std::endl;

    std::cout << "input                 std::hash          " << HashName << std::endl;
    for (T i = 0; i < 100; ++i) {
        showHash(i);
    }

    for (T i = 0; i < 10; ++i) {
        T s = ((0x23d7 + i) << (sizeof(T) * 4)) + 12;
        showHash(s);
    }

    // <= 0 so it works with int overflows
    for (uint64_t i = 1; i <= (std::numeric_limits<T>::max)() && i != 0; i *= 2) {
        showHash(static_cast<T>(i));
    }
    for (uint64_t i = 1; i <= (std::numeric_limits<T>::max)() && i != 0; i *= 2) {
        showHash(static_cast<T>(i + 1));
    }
}

BENCHMARK(ShowHash) {
    showHash<uint64_t>("uint64_t");
    showHash<int32_t>("int32_t");
}
