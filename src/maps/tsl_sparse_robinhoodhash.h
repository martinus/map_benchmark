#pragma once

#include "hashes.h"
#include "robin_hood/robin_hood.h"
#include "tsl/sparse_map.h"

static const char* MapName = "tsl::sparse_map robin_hood::hash";

template <class Key, class Val>
using Map = tsl::sparse_map<Key, Val, robin_hood::hash<Key>>;
