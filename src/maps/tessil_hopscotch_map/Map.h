#pragma once

#include "Hash.h"
#include "Tessil__hopscotch-map/include/tsl/hopscotch_map.h"

static const char* MapName = "tsl::hopscotch_map";

template <class Key, class Val>
using Map = tsl::hopscotch_map<Key, Val, Hash<Key>>;
