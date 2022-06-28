#pragma once

#include "Hash.h"
#include "martinus__unordered_dense_map/include/ankerl/unordered_dense_map.h"

static const char* MapName = "ankerl::unordered_dense_map";

template <class Key, class Val>
using Map = ankerl::unordered_dense_map<Key, Val, Hash<Key>>;
