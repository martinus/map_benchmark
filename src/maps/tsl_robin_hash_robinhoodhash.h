#pragma once

#include "hashes.h"
#include "robin_hood/robin_hood.h"
#include "tsl/robin_map.h"

static const char* MapName = "tsl::robin_map robin_hood::hash";

template <class Key, class Val>
using Map = tsl::robin_map<Key, Val, robin_hood::hash<Key>>;
