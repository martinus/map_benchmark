#pragma once

#include "robin_hood/robin_hood.h"
#include <unordered_map>

static const char* MapName = "std::unordered_map robin_hood::hash";

template <class Key, class Val>
using Map = std::unordered_map<Key, Val, robin_hood::hash<Key>>;
