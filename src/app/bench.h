#pragma once

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

struct BenchTitleException {};

class Bench {
public:
	Bench(size_t numTrials = 5, uint64_t seed = 123)
		: mSeed(seed)
		, mNumTrials(numTrials)
		, mCurrentTrial(0)
		, mResult(0)
		, mThrowAfterTitle(false) {}

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

	Bench& description(const std::string& txt) {
		mDescription = txt;
		return *this;
	}

	const std::string& description() const {
		return mDescription;
	}

	Bench& result(uint64_t r) {
		mResult = r;
		return *this;
	}

	Bench& event(const char* msg) {
		if (mPeriodicMemoryStats) {
			mPeriodicMemoryStats->event(msg);
		}
	}

	sfc64& rng() {
		return mRng;
	}

	inline void beginMeasure() {
#ifdef ENABLE_MALLOC_HOOK
		mPeriodicMemoryStats = std::make_unique<PeriodicMemoryStats>(0.05);
#endif
		mTimePoint = std::chrono::high_resolution_clock::now();
	}

	PeriodicMemoryStats* periodicMemoryStats() {
		return mPeriodicMemoryStats.get();
	}

	inline void endMeasure() {
		std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
		if (mPeriodicMemoryStats) {
			mPeriodicMemoryStats->stop();
		}

		std::chrono::duration<double> diff = now - mTimePoint;

		mRuntimeSec = diff.count();
	}

	double runtimeSeconds() const {
		return mRuntimeSec;
	}

	std::string str() const {
		size_t s = 0;
		for (auto const& x : mRng.state()) {
			hash_combine(s, x);
		}
		hash_combine(s, mResult);

		std::stringstream ss;
		ss << std::setw(10) << std::right << mRuntimeSec << "s " << std::hex << "0x" << s << " " << title();

		return ss.str();
	}

private:
	uint64_t const mSeed;
	sfc64 mRng;
	std::string mTitle;
	size_t const mNumTrials;
	size_t mCurrentTrial;
	std::chrono::high_resolution_clock::time_point mTimePoint;
	uint64_t mResult;
	bool mThrowAfterTitle;
	double mRuntimeSec;
	std::string mDescription;
	std::unique_ptr<PeriodicMemoryStats> mPeriodicMemoryStats;
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
