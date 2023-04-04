#pragma once

#define BOOST_UNORDERED_DISABLE_SSE2
#define BOOST_UNORDERED_DISABLE_NEON

#include "Hash.h"
//#include <boost__unordered_flat_map/include/boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/version.hpp>

static const char* MapName = "boost::unordered_flat_map no SSE " BOOST_LIB_VERSION;

template <class Key, class Val>
using Map = boost::unordered_flat_map<Key, Val, Hash<Key>>;
