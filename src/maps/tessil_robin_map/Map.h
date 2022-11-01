#pragma once

#include "Hash.h"
#include "Tessil__robin-map/include/tsl/robin_map.h"

static const char* MapName = "tsl::robin_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = tsl::robin_map<Key, Val, H>;
