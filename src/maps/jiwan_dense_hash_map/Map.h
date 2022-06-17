#pragma once

#include "Hash.h"
#include "jiwan__dense_hash_map/include/jg/dense_hash_map.hpp"

static const char* MapName = "jg::dense_hash_map";

template <class Key, class Val>
using Map = jg::dense_hash_map<Key, Val, Hash<Key>>;
