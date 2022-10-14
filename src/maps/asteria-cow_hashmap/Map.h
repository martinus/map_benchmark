#pragma once

#include "pool.h"
#include "Hash.h"

// This is an evil hack for the absence of `emplace()`.
#define try_emplace  emplace

#define ROCKET_NO_STRICT_HASH_NOEXCEPT
#include "lhmouse__asteria/rocket/cow_hashmap.hpp"

// We need `::rocket::sprintf_and_throw<::std::out_of_range>()`.
#include "lhmouse__asteria/rocket/throw.cpp"

static const char* MapName = "asteria::cow_hashmap";

template<class Key, class Val>
using Map = ::rocket::cow_hashmap<Key, Val, Hash<Key>>;
