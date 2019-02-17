#pragma once

#include "Hash.h"
#include "folly/container/F14Map.h"

static const char* MapName = "folly::F14FastMap";

// TODO add hash
template <class Key, class Val>
using Map = folly::F14FastMap<Key, Val>;
