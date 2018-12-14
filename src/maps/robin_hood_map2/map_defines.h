#pragma once

#include "robin_hood_map.h"

static const char* MapName = "robin_hood::map";

template <class Key, class Val>
using Map = robin_hood::map<Key, Val>;
