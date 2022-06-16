#pragma once

#include "Hash.h"
#include "ktprime__emhash/hash_table8.hpp"

static const char* MapName = "emhash8::HashMap";

template <class Key, class Val>
using Map = emhash8::HashMap<Key, Val, Hash<Key>>;
