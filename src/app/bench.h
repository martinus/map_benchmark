#pragma once

#include <chrono>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <unordered_map>

class Bench {
public:
    using clock = std::chrono::high_resolution_clock;

    inline void beginMeasure(std::initializer_list<const char*> tags) {
        show_tags(tags);
        mStartTime = clock::now();
    }

    void endMeasure(uint64_t expected_result, uint64_t actual_result) {
        auto const end = clock::now();
        show_result(end, expected_result, actual_result);
    }

private:
    void show_tags(std::initializer_list<const char*> tags) {
        for (auto tag : tags) {
            std::cout << "\"" << tag << "\"" << mSep;
        }
    }

    void show_result(clock::time_point end, uint64_t expected_result, uint64_t actual_result) {
        std::chrono::duration<double> duration = end - mStartTime;

        auto runtime_sec = duration.count();
        std::cout << expected_result << mSep << runtime_sec << std::endl;
        if (actual_result != expected_result) {
            std::cerr << "ERROR: expected " << expected_result << " but got " << actual_result << std::endl;
        }
    }

    clock::time_point mStartTime;
    const char* mSep = "; ";
};

class BenchRegistry {
public:
    using Fn = std::function<void(Bench&)>;
    using NameToFn = std::map<std::string, Fn>;

    BenchRegistry(const char* name, Fn fn) {
        if (!nameToFn().emplace(name, fn).second) {
            std::cerr << "benchmark with name '" << name << "' already exists!" << std::endl;
            throw std::exception();
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

    static int run(const char* name) {
        auto it = BenchRegistry::nameToFn().find(name);
        if (it == BenchRegistry::nameToFn().end()) {
            return -1;
        }
        Bench bench;
        it->second(bench);
        return 0;
    }
};

#define BENCHMARK_PP_CAT(a, b) BENCHMARK_PP_INTERNAL_CAT(a, b)
#define BENCHMARK_PP_INTERNAL_CAT(a, b) a##b
#define BENCHMARK(f)                                                                                                                                 \
    static void f(Bench& bench);                                                                                                                     \
    static BenchRegistry BENCHMARK_PP_CAT(reg, __LINE__)(#f, f);                                                                                     \
    static void f(Bench& bench)
