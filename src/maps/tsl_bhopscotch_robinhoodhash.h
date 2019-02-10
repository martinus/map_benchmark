#pragma once

#include "hashes.h"
#include "robin_hood/robin_hood.h"
#include "tsl/bhopscotch_map.h"

static const char* MapName = "tsl::bhopscotch_map robin_hood::hash";

template <class Key, class Val>
using Map = tsl::bhopscotch_map<Key, Val, robin_hood::hash<Key>>;
