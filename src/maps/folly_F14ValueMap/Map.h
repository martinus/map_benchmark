#pragma once

#include "Hash.h"
#include "folly/container/F14Map.h"

static const char* MapName = "folly::F14ValueMap";

template <class Key, class Val>
using Map = folly::F14ValueMap<Key, Val, Hash<Key>>;
