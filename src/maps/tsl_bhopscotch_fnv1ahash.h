#pragma once

#include "hashes.h"
#include "tsl/bhopscotch_map.h"

static const char* MapName = "tsl::bhopscotch hash::FNV1a";

template <class Key, class Val>
using Map = tsl::bhopscotch_map<Key, Val, hash::FNV1a<Key>>;
