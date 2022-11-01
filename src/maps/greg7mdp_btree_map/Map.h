#pragma once

#include "Hash.h"
#include "greg7mdp__gtl/include/gtl/btree.hpp"

static const char* MapName = "gtl::btree_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = gtl::btree_map<Key, Val>;
