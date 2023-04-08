#pragma once

#include "Hash.h"
#include "martinus__unordered_dense/include/ankerl/unordered_dense.h"

static const char* MapName = "ankerl::unordered_dense::segmented_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = ankerl::unordered_dense::segmented_map<Key, Val, H>;
