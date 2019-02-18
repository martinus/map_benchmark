#pragma once

#include "Map.h"

#include <MapHash.h>
#include <PeriodicMemoryStats.h>
#include <sfc64.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

class Bench {
public:
    using clock = std::chrono::high_resolution_clock;

    template <typename Op>
    void operator()(std::initializer_list<const char*> tags, uint64_t expected_result, Op&& op) {
        auto const begin = clock::now();
        op();
        auto const end = clock::now();
        show_result(tags, expected_result, begin, end);
    }

private:
    void show_result(std::initializer_list<const char*> tags, uint64_t expected_result, clock::time_point const& begin,
                     clock::time_point const& end) {
        std::chrono::duration<double> duration = end - begin;

        const char* sep = "; ";
        auto runtime_sec = duration.count();
        for (auto tag : tags) {
            std::cout << "\"" << tag << "\"" << sep;
        }
        std::cout << expected_result << sep << runtime_sec << std::endl;
    }
};

#include <map>

class BenchRegister {
public:
    using NameToFn = std::map<std::string, std::function<void(Bench&)>>;

    template <class... Fn>
    BenchRegister(Fn&&... fns) {
        for (auto& fn : {fns...}) {
            Bench bench;
            bench.throwAfterTitle();
            try {
                fn(bench);
            } catch (const BenchTitleException&) {
                if (!nameToFn().emplace(bench.title(), fn).second) {
                    std::cerr << "benchmark with title '" << bench.title() << "' already exists!" << std::endl;
                    throw std::exception();
                }
            }
        }
    }

    static void list() {
        for (auto const& nameFn : nameToFn()) {
            std::cout << nameFn.first << std::endl;
        }
    }

    static NameToFn& nameToFn() {
        static NameToFn sNameToFn;
        return sNameToFn;
    }
};
