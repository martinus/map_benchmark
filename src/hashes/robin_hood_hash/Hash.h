#pragma once

#include "martinus__robin-hood-hashing/src/include/robin_hood.h"

static const char* HashName = "robin_hood::hash";

template <class Key>
using Hash = robin_hood::hash<Key>;
