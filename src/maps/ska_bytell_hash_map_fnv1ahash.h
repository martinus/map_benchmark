#pragma once

#include "hashes.h"
#include "ska/bytell_hash_map.hpp"

static const char* MapName = "ska::bytell_hash_map hash::FNV1a";

template <class Key, class Val>
using Map = ska::bytell_hash_map<Key, Val, hash::FNV1a<Key>>;
