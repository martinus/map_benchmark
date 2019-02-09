#pragma once

#include "absl/absl/container/flat_hash_map.h"
#include "robin_hood/robin_hood.h"

static const char* MapName = "absl::flat_hash_map robin_hood::hash";

template <class Key, class Val>
using Map = absl::flat_hash_map<Key, Val, robin_hood::hash<Key>>;
