#pragma once

#include "martinus__unordered_dense_map/include/ankerl/unordered_dense_map.h"

static const char* HashName = "ankerl::hash";

template <class Key>
using Hash = ankerl::hash<Key>;
