#pragma once

#include "robin_hood.h"

static const char* MapName = "robin_hood::unordered_map";

template <class Key, class Val>
using Map = robin_hood::unordered_map<Key, Val>;
