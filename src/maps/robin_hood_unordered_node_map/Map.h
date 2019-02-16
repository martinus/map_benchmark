#pragma once

#include "Hash.h"
#include "robin_hood/robin_hood.h"

static const char* MapName = "robin_hood::unordered_node_map";

template <class Key, class Val>
using Map = robin_hood::unordered_node_map<Key, Val, Hash<Key>>;
