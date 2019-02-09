#pragma once

#include "hashes.h"
#include <unordered_map>

static const char* MapName = "std::unordered_map hash::FVN1a";

template <class Key, class Val>
using Map = std::unordered_map<Key, Val, hash::FNV1a<Key>>;
