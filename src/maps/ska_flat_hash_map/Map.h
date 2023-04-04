#pragma once

#include "Hash.h"
#include "skarupke__flat_hash_map/flat_hash_map.hpp"

static const char* MapName = "ska::flat_hash_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = ska::flat_hash_map<Key, Val, H>;
