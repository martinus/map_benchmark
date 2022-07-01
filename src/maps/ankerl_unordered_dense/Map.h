#pragma once

#include "Hash.h"
#include "martinus__unordered_dense/include/ankerl/unordered_dense.h"

static const char* MapName = "ankerl::unordered_dense::map";

template <class Key, class Val>
using Map = ankerl::unordered_dense::map<Key, Val, Hash<Key>>;
