#pragma once

#include "absl/absl/hash/hash.h"
#include "robin_hood/robin_hood.h"

static const char* MapName = "robin_hood::unordered_flat_map absl::Hash";

template <class Key, class Val>
using Map = robin_hood::unordered_flat_map<Key, Val, absl::Hash<Key>>;
