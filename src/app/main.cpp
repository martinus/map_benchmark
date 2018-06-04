#include "map_defines.h"

#include <XoRoShiRo128Plus.h>
#include <MapHash.h>

#include <iostream>
#include <iomanip>
#include <chrono>

int main(int, char**) {
    Map<int, int> m;

    XoRoShiRo128Plus rng(123);

    auto start = std::chrono::high_resolution_clock::now();
    const size_t iters = 10'000'000;
    for (size_t i=0; i<iters; ++i) {
        m[rng(10'000)] = i;
        m.erase(rng(10'000));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    size_t const h = mapHash(m);

    std::cout 
        << std::setw(10) << std::right << (diff.count() * 1e9 / iters) << "ns " 
        << m.size() << " " 
        << std::hex << "0x" << rng() << " " 
        << std::hex << "0x" << h << " " 
        << MapName << std::endl;
}