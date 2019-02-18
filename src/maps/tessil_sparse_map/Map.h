#pragma once

#include "Hash.h"
#include "Tessil__sparse-map/include/tsl/sparse_map.h"

static const char* MapName = "tsl::sparse_map";

template <class Key, class Val>
using Map = tsl::sparse_map<Key, Val, Hash<Key>>;
