#pragma once

#include "Hash.h"
#include "fph__noseed_fph_map/include/fph/dynamic_fph_table.h"

static const char* MapName = "fph::DynamicFphMap";

template <class Key, class Val>
using Map = fph::DynamicFphMap<Key, Val, Hash<Key>>;