#pragma once

#include "boost/functional/hash.hpp"

static const char* HashName = "boost::hash";

template <typename Key>
using Hash = boost::hash<Key>;
