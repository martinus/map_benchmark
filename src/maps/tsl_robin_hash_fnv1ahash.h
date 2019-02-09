#pragma once

#include "hashes.h"
#include "tsl/robin_map.h"

static const char* MapName = "tsl::robin_map hash::FNV1a";

template <class Key, class Val>
using Map = tsl::robin_map<Key, Val, hash::FNV1a<Key>>;
