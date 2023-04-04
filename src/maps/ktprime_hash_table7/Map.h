#pragma once

#include "Hash.h"
#include "ktprime__emhash/hash_table7.hpp"

static const char* MapName = "emhash7::HashMap";

template <class Key, class Val, class H = Hash<Key>>
using Map = emhash7::HashMap<Key, Val, H>;
