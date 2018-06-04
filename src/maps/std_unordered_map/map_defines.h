#pragma once

#include <unordered_map>

const char* MapName = "std::unordered_map";

template <class Key, class Val>
using Map = std::unordered_map<Key, Val>;
