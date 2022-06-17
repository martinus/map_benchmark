#pragma once

#include "Hash.h"
#include "pool.h"

#include <unordered_map>

static const char* MapName = "std::unordered_map PoolAllocator";

#define USE_POOL_ALLOCATOR 1

template <class Key, class Val>
using Map = std::unordered_map<Key, Val, Hash<Key>, std::equal_to<Key>,
                               PoolAllocator<std::pair<const Key, Val>, sizeof(std::pair<const Key, Val>) + sizeof(void*) * 4, alignof(void*)>>;

template <class Key, class Val>
using Resource = typename Map<Key, Val>::allocator_type::ResourceType;