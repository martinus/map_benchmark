#pragma once

#include "Hash.h"
#include <boost/unordered_map.hpp>

static const char* MapName = "boost::unordered_map";

template <class Key, class Val>
using Map = boost::unordered_map<Key, Val, Hash<Key>>;
