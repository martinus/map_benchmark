#pragma once

#include "RobinHoodMap.h"

static const char* MapName = "rh::Map";

template <class Key, class Val>
using Map = rh::Map<Key, Val>;
