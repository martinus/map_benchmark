#pragma once

#include <map>

const char* MapName = "std::map";

template <class Key, class Val>
using Map = std::map<Key, Val>;
