#pragma once

#include "folly/hash/Hash.h"

static const char* HashName = "folly::hasher";

template <typename Key>
using Hash = folly::hasher<Key>;
