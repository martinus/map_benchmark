#pragma once

#include "absl/absl/container/node_hash_map.h"
#include "robin_hood/robin_hood.h"

static const char* MapName = "absl::node_hash_map robin_hood::hash";

template <class Key, class Val>
using Map = absl::node_hash_map<Key, Val, robin_hood::hash<Key>>;
