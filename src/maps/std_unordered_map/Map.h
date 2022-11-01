#pragma once

#include "pool.h"
#include "Hash.h"
#include <unordered_map>

static const char* MapName = "std::unordered_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = std::unordered_map<Key, Val, H>;
