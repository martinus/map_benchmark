#pragma once

#include "Hash.h"
#include "absl/container/flat_hash_map.h"

static const char* MapName = "absl::flat_hash_map";

template <class Key, class Val>
using Map = absl::flat_hash_map<Key, Val, Hash<Key>>;
