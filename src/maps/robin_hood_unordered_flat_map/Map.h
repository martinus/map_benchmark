#pragma once

#include "Hash.h"
#include "martinus__robin-hood-hashing/src/include/robin_hood.h"

static const char* MapName = "robin_hood::unordered_flat_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = robin_hood::unordered_flat_map<Key, Val, H>;
