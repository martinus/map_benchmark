#pragma once

#include "Hash.h"
#include "greg7mdp__sparsepp/sparsepp/spp.h"

static const char* MapName = "spp::sparse_hash_map";

template <class Key, class Val, class H = Hash<Key>>
using Map = spp::sparse_hash_map<Key, Val, H>;
