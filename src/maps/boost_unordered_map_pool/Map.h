#pragma once

#include "Hash.h"
#include "pool.h"

#include <boost/unordered_map.hpp>
#include <boost/version.hpp>

static const char* MapName = "boost::unordered_map PoolAllocator " BOOST_LIB_VERSION;

#define USE_POOL_ALLOCATOR 1

template <class Key, class Val>
using Map = boost::unordered_map<Key, Val, Hash<Key>, std::equal_to<Key>,
                                 PoolAllocator<std::pair<const Key, Val>, sizeof(std::pair<const Key, Val>) + sizeof(void*) * 4, alignof(void*)>>;

template <class Key, class Val>
using Resource = typename Map<Key, Val>::allocator_type::ResourceType;
