#pragma once

#include "hashes.h"
#include "robin_hood/robin_hood.h"
#include "tsl/hopscotch_map.h"

static const char* MapName = "tsl::hopscotch_map robin_hood::hash";

template <class Key, class Val>
using Map = tsl::hopscotch_map<Key, Val, robin_hood::hash<Key>>;
