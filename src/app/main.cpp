#include "map_defines.h"

#include <XoRoShiRo128Plus.h>

#include <iostream>
#include <iomanip>
#include <chrono>

int main(int, char**) {
    Map<int, int> m;

    XoRoShiRo128Plus rng(123);

    auto start = std::chrono::high_resolution_clock::now();
    const size_t iters = static_cast<size_t>(100000000 * MapScale);
    for (size_t i=0; i<iters; ++i) {
        m[rng(10000)] = i;
        m.erase(rng(10000));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << std::setw(10) << std::right << (diff.count() * 1e9 / iters) << "ns " << m.size() << " " << rng() << " " << MapName << std::endl;
}