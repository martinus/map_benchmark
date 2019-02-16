#pragma once

#include "robin_hood/robin_hood.h"

static const char* HashName = "robin_hood::hash";

template <class Key>
using Hash = robin_hood::hash<Key>;
