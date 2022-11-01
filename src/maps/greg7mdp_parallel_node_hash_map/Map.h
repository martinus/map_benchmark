#pragma once

#include "Hash.h"
#include "greg7mdp__gtl/include/gtl/phmap.hpp"

static const char* MapName = "gtl::parallel_node_hash_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = gtl::parallel_node_hash_map<Key, Val, H>;
