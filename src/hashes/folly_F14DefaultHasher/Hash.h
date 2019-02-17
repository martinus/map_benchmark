#pragma once

#include "folly/container/F14Map.h"

static const char* HashName = "folly::f14::DefaultHasher";

template <typename Key>
using Hash = folly::f14::DefaultHasher<Key>;
