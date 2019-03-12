#pragma once

#include "Hash.h"
#include "ktprime__ktprime/hash_table5.hpp"

static const char* MapName = "emilib1::HashMap";

template <class Key, class Val>
using Map = emilib1::HashMap<Key, Val, Hash<Key>>;
