#pragma once

#include "martinus__unordered_dense/include/ankerl/unordered_dense.h"

static const char* HashName = "ankerl::unordered_dense::hash";

template <class Key>
using Hash = ankerl::unordered_dense::hash<Key>;
