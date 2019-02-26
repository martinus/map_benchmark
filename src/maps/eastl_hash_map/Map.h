#pragma once

#include "EASTL/hash_map.h"
#include "Hash.h"

static const char* MapName = "eastl::hash_map";

template <class Key, class Val>
using Map = eastl::hash_map<Key, Val, Hash<Key>>;
