#pragma once

#include "absl/hash/hash.h"

static const char* HashName = "absl::Hash";

template <typename Key>
using Hash = absl::Hash<Key>;