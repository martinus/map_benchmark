#pragma once

#include "hashes.h"
#include "tsl/bhopscotch_map.h"

static const char* MapName = "tsl::bhopscotch_map hash::Null";

template <class Key, class Val>
using Map = tsl::bhopscotch_map<Key, Val, hash::Null<Key>>;
