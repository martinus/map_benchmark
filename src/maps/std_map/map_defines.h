#pragma once

#include <map>

static const char *MapName = "std::map";

template <class Key, class Val> using Map = std::map<Key, Val>;
