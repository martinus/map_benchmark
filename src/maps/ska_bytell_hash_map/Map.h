#pragma once

#include <limits>

#include "Hash.h"
#include "skarupke__flat_hash_map/bytell_hash_map.hpp"

static const char* MapName = "ska::bytell_hash_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = ska::bytell_hash_map<Key, Val, H>;
