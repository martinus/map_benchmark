#pragma once

#include "Hash.h"
#include "pool.h"

#include <unordered_map>

static const char* MapName = "std::unordered_map PoolAllocator";

#define USE_POOL_ALLOCATOR 1

template <class Key, class Val, class H = Hash<Key>>
using Map = std::unordered_map<Key, Val, H, std::equal_to<Key>,
                               PoolAllocator<std::pair<const Key, Val>, sizeof(std::pair<const Key, Val>) + sizeof(void*) * 4, alignof(void*)>>;

template <class Key, class Val, class H = Hash<Key>>
using Resource = typename Map<Key, Val, H>::allocator_type::ResourceType;