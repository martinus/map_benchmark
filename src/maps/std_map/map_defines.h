#pragma once

#include <map>

const char* MapName = "std::map";
const double MapScale = 0.25;


template <class Key, class Val>
using Map = std::map<Key, Val>;
