#pragma once

#include "hashes.h"
#include "robin_hood/robin_hood.h"

static const char* MapName = "robin_hood::unordered_flat_map robin_hood::hash";

template <class Key, class Val>
using Map = robin_hood::unordered_flat_map<Key, Val, robin_hood::hash<Key>>;
