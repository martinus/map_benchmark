#pragma once

#include "robin_hood/robin_hood.h"
#include "ska/bytell_hash_map.hpp"

static const char* MapName = "ska::bytell_hash_map robin_hood::hash";

template <class Key, class Val>
using Map = ska::bytell_hash_map<Key, Val, robin_hood::hash<Key>>;
