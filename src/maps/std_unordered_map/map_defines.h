#pragma once

#include <unordered_map>

const char* MapName = "std::unordered_map";
const double MapScale = 1.0;

template <class Key, class Val>
using Map = std::unordered_map<Key, Val>;
