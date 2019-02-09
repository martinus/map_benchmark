#pragma once

#include "hashes.h"
#include <unordered_map>

static const char* MapName = "std::unordered_map hash::Null";

template <class Key, class Val>
using Map = std::unordered_map<Key, Val, hash::Null<Key>>;
