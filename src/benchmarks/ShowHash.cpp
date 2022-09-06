#include "Hash.h"
#include "bench.h"
#include "hex.h"
#include "sfc64.h"

#include <bitset>

template <typename T>
void showHash(T val) {
    auto sh = std::hash<T>{}(val);
    auto mh = Hash<T>{}(val);
    std::cerr << hex(val) << " -> " << hex(sh) << " " << hex(mh) << " " << std::bitset<sizeof(size_t) * 8>(mh) << std::endl;
}

template <typename T>
void showHash(const char* title) {
    std::cerr << std::endl << title << std::endl;

    std::cerr << "input                 std::hash          " << HashName << std::endl;
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

void showHashString(std::string const& val) {
    auto mh = Hash<std::string>{}(val);
    auto showVal = '"' + val + '"';
    std::cerr << std::setfill(' ') << std::setw(14) << showVal << " -> " << hex(mh) << " " << std::bitset<sizeof(size_t) * 8>(mh) << std::endl;
}

BENCHMARK(ShowHashString) {
    showHashString("a");
    showHashString("aa");
    showHashString("aaa");
    showHashString("b");
    showHashString("c");
    showHashString("aaaaaaaaaaa");
    showHashString("aaaaaaaaaab");
    showHashString("aaaaaaaaaac");
    showHashString("baaaaaaaaaa");
    showHashString("caaaaaaaaaa");
}

BENCHMARK(ShowIsStableReference) {
    using M = Map<uint64_t, uint64_t>;
#ifdef USE_POOL_ALLOCATOR
    Resource<uint64_t, uint64_t> resource;
    M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M map;
#endif

    auto keyToVal = std::unordered_map<uint64_t const*, uint64_t>();
    auto mappedToVal = std::unordered_map<uint64_t const*, uint64_t>();

    auto rng = sfc64(987654);
    for (size_t i = 0; i < 10000; ++i) {
        auto val = rng();
        map[val] = val;

        auto it = map.find(val);
        keyToVal[&it->first] = it->first;
        mappedToVal[&it->second] = it->second;
    }

    auto isStable = true;
    for (auto const& kv : map) {
        auto itKey = keyToVal.find(&kv.first);
        if (itKey == keyToVal.end() || itKey->second != kv.first) {
            isStable = false;
            break;
        }

        auto itMapped = mappedToVal.find(&kv.second);
        if (itMapped == mappedToVal.end() || itMapped->second != kv.second) {
            isStable = false;
            break;
        }
    }

    std::cerr << isStable << " " << MapName << std::endl;
}