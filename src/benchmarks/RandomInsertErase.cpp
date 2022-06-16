#include "Map.h"
#include "bench.h"
#include "hex.h"
#include "sfc64.h"

#include <algorithm>
#include <bitset>
#include <numeric>
#include <sstream>

template <typename T>
struct as_bits_t {
    T value;
};

template <typename T>
as_bits_t<T> as_bits(T value) {
    return as_bits_t<T>{value};
}

template <typename T>
std::ostream& operator<<(std::ostream& os, as_bits_t<T> const& t) {
    os << std::bitset<sizeof(t.value) * 8>(t.value);
    return os;
}

BENCHMARK(RandomInsertErase) {
    // random bits to set for the mask
    std::vector<int> bits(64);
    std::iota(bits.begin(), bits.end(), 0);
    sfc64 rng(999);
    std::shuffle(bits.begin(), bits.end(), rng);

    uint64_t bitMask = 0;
    auto bitsIt = bits.begin();

    size_t const expectedFinalSizes[] = {7, 127, 2084, 32722, 524149, 8367491};
    size_t const max_n = 50'000'000;

    Map<uint64_t, uint64_t> map;
    for (int i = 0; i < 6; ++i) {
        // each iteration, set 4 new random bits.
        for (int b = 0; b < 4; ++b) {
            bitMask |= UINT64_C(1) << *bitsIt++;
        }

        // std::cout << (i + 1) << ". " << as_bits(bitMask) << std::endl;

        // set name
        size_t verifier = 0;
        std::stringstream ss;
        ss << std::bitset<64>(bitMask).count() << " bits, " << (max_n / 1000'000) << "M cycles";

        // benchmark randomly inserting & erasing
        bench.beginMeasure(ss.str().c_str());
        for (size_t i = 0; i < max_n; ++i) {
            map.emplace(rng() & bitMask, i);
            map.erase(rng() & bitMask);
        }
        bench.endMeasure(expectedFinalSizes[i], map.size());
    }
}
