#pragma once

#define LOG_HASH

#include "Hash.h"
#include "ktprime__ktprime/hash_table5.hpp"

static const char* MapName = "emilib4::HashMap";

template <class Key, class Val>
using Map = emilib4::HashMap<Key, Val, Hash<Key>>;
