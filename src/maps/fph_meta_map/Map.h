#pragma once

#include "Hash.h"
#include "fph__noseed_fph_map/include/fph/meta_fph_table.h"

static const char* MapName = "fph::MetaFphMap";

template <class Key, class Val>
using Map = fph::MetaFphMap<Key, Val, Hash<Key>>;