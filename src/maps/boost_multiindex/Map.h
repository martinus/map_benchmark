#pragma once

#include "Hash.h"

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>

#include <utility>

static const char* MapName = "boost::multi_index::hashed_unique";

template <class T1, class T2>
struct MapElement {
    using first_type = T1;
    using second_type = T2;

    template <class Q1>
    MapElement(Q1&& first)
        : first{std::forward<Q1>(first)}
        , second{} {}

    template <class Q1, class Q2>
    MapElement(Q1&& first, Q2&& second)
        : first{std::forward<Q1>(first)}
        , second{std::forward<Q2>(second)} {}

    T1 first;
    mutable T2 second;
};

template <class Key, class Val>
using MapBase = boost::multi_index_container<MapElement<Key, Val>,
                                             boost::multi_index::indexed_by<boost::multi_index::hashed_unique<
                                                 boost::multi_index::member<MapElement<Key, Val>, Key, &MapElement<Key, Val>::first>, Hash<Key>>>>;

template <class Key, class Val>
struct Map : MapBase<Key, Val> {
    using super = MapBase<Key, Val>;
    using super::super;

    template <class K>
    Val& operator[](K&& key) {
        return this->emplace(std::forward<K>(key)).first->second;
    }
};