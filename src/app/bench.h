#pragma once

#include <XoRoShiRo128Plus.h>
#include <MapHash.h>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <sstream>
#include <algorithm>

struct BenchTitleException {};

class Bench {
public:
    Bench(size_t numTrials = 5, uint64_t seed = 123)
        : mSeed(seed)
        , mNumTrials(numTrials)
        , mCurrentTrial(0)
        , mResult(0)
        , mThrowAfterTitle(false)
    { }

    void throwAfterTitle() {
        mThrowAfterTitle = true;        
    }

    Bench& title(const std::string& txt) {
        mTitle = txt;
        if (mThrowAfterTitle) {
            throw BenchTitleException{};
        }
        return *this;
    }

    std::string const& title() const {
        return mTitle;
    }

    Bench& result(uint64_t r) {
        mResult = r;
        return *this;
    }

    XoRoShiRo128Plus& rng() {
        return mRng;
    }

    void beginMeasure() {
        mTimePoint = std::chrono::high_resolution_clock::now();
    }

    void endMeasure() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = now - mTimePoint;
        mRuntimeSec = diff.count();
    }

    double runtimeSeconds() const {
        return mRuntimeSec;
    }

    std::string str() const {
        auto const& state = mRng.state();

        size_t s = 0;
        hash_combine(s, state.first);
        hash_combine(s, state.second);        
        hash_combine(s, mResult);

        std::stringstream ss;
        ss << std::setw(10) << std::right << mRuntimeSec << "s " 
            << std::hex << "0x" << s << " " << MapName;

        return ss.str();
    }

private:
    uint64_t const mSeed;
    XoRoShiRo128Plus mRng;
    std::string mTitle;
    size_t const mNumTrials;
    size_t mCurrentTrial;
    std::chrono::high_resolution_clock::time_point mTimePoint;
    uint64_t mResult;
    bool mThrowAfterTitle;
    double mRuntimeSec;
};

#include <map>

class BenchRegister {
public:
    using NameToFn = std::map<std::string, std::function<void(Bench&)>>;

    BenchRegister(std::function<void(Bench&)> fn) {
        Bench bench;
        bench.throwAfterTitle();
        try {
            fn(bench);
        } catch (const BenchTitleException&) {
            nameToFn().emplace(bench.title(), fn);
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
