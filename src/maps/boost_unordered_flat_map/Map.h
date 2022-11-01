#pragma once

#include "Hash.h"
#include <boost__unordered_flat_map/include/boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/version.hpp>

static const char* MapName = "boost::unordered_flat_map " BOOST_LIB_VERSION;

template <class Key, class Val, class H = Hash<Key>>
using Map = boost::unordered_flat_map<Key, Val, H>;
