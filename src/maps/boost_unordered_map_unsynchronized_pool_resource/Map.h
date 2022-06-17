#pragma once

#include "Hash.h"
#include "pool.h"

#include <boost/container/allocator.hpp>
#include <boost/container/detail/multiallocation_chain.hpp>
#include <boost/container/node_allocator.hpp>
#include <boost/container/pmr/polymorphic_allocator.hpp>
#include <boost/container/pmr/unsynchronized_pool_resource.hpp>
#include <boost/unordered_map.hpp>
#include <boost/version.hpp>

static const char* MapName = "boost::unordered_map unsynchronized_pool_resource " BOOST_LIB_VERSION;

#define USE_POOL_ALLOCATOR 1

template <class Key, class Val>
using Map = boost::unordered_map<Key, Val, Hash<Key>, std::equal_to<Key>, boost::container::pmr::polymorphic_allocator<std::pair<const Key, Val>>>;

template <class Key, class Val>
using Resource = boost::container::pmr::unsynchronized_pool_resource;


#if 0
template <class Key, class Val>
using Map = boost::unordered_map<Key, Val, Hash<Key>, std::equal_to<Key>, boost::container::node_allocator<std::pair<const Key, Val>>>;

#endif