#pragma once

#include "hashes.h"
#include <unordered_map>

static const char* MapName = "std::unordered_map hash::CityHash";

template <class Key, class Val>
using Map = std::unordered_map<Key, Val, hash::CityHash<Key>>;
