#pragma once

#include <limits>
#include "Hash.h"
#include "rigtorp__HashMap/include/rigtorp/HashMap.h"

static const char* MapName = "rigtorp::HashMap";

template <class Key, class Val>
struct Map : public rigtorp::HashMap<Key, Val, Hash<Key> > {
	// This hash map requires a sentinel key, which can't be used with the
	// map. Fortunately, using the maximum value of the Key type happens to
	// work with the benchmark.
	// This may break in the future!
	Map() : rigtorp::HashMap<Key, Val, Hash<Key> >(16, std::numeric_limits<Key>::max()) { }
};

template <class Val>
struct Map<std::string, Val> : public rigtorp::HashMap<std::string, Val, Hash<std::string> > {
	// none of the string benchmarks use the empty string,
	// so we can use it as the empty key sentinel.
	Map() : rigtorp::HashMap<std::string, Val, Hash<std::string> >(16, std::string()) { }
};
