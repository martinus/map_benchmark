#pragma once

#include "absl/absl/container/node_hash_map.h"
#include "hashes.h"

static const char* MapName = "absl::node_hash_map std::hash";

template <class Key, class Val>
using Map = absl::node_hash_map<Key, Val, std::hash<Key>>;
