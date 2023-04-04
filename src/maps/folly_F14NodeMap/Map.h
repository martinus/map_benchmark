#pragma once

#include "Hash.h"
#include "folly/container/F14Map.h"

static const char* MapName = "folly::F14NodeMap";

template <class Key, class Val, class H = Hash<Key>>
using Map = folly::F14NodeMap<Key, Val, H>;
