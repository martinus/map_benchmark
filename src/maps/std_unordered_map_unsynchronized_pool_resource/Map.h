#pragma once

#include "Hash.h"
#include "pool.h"

#include <memory_resource>
#include <unordered_map>

static const char* MapName = "std::unordered_map unsynchronized_pool_resource";

#define USE_POOL_ALLOCATOR 1

template <class Key, class Val>
using Map = std::pmr::unordered_map<Key, Val>;

template <class Key, class Val>
using Resource = std::pmr::unsynchronized_pool_resource;
