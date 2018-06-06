#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include "Windows.h"
#include <psapi.h>
#endif

class PeriodicMemoryStats {
public:
	struct Stats {
		std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
		size_t memPrivateUsage;

		bool operator<(const Stats& o) const {
			if (timestamp < o.timestamp) {
				return true;
			} else if (o.timestamp < timestamp) {
				return false;
			} else {
				return memPrivateUsage < o.memPrivateUsage;
			}
		}
	};

	PeriodicMemoryStats(double intervalSeconds)
		: mIntervalMicros(static_cast<uint64_t>(intervalSeconds * 1000 * 1000))
		, mDoContinue(true)
		, mThread(&PeriodicMemoryStats::runner, this) {}

	~PeriodicMemoryStats() {
		stop();
	}

	void stop() {
		if (mDoContinue) {
			mDoContinue = false;
			mThread.join();
		}

		for (const auto e : mEvents) {
			mPeriodic.push_back(e.first);
		}
		std::sort(mPeriodic.begin(), mPeriodic.end());
	}

	void event(const char* title) {
		Stats s;
		s.timestamp = std::chrono::high_resolution_clock::now();
		s.memPrivateUsage = getMem();
		mEvents.emplace_back(std::move(s), title);
	}

	void plotly(std::vector<std::string>& linePositionsX, std::vector<std::string>& linePositionsY, std::vector<std::string>& markerPositionsX,
				std::vector<std::string>& markerPositionsY, std::vector<std::string>& markerText) const {

		auto begin = mPeriodic[0].timestamp;
		if (!mEvents.empty()) {
			begin = (std::min)(begin, mEvents[0].first.timestamp);

			for (size_t i = 0; i < mEvents.size(); ++i) {
				const PeriodicMemoryStats::Stats& s = mEvents[i].first;
				std::chrono::duration<double> const diff = s.timestamp - begin;
				markerPositionsX.push_back(std::to_string(diff.count()));
				markerPositionsY.push_back(std::to_string((s.memPrivateUsage / (1024.0 * 1024))));
				markerText.push_back(mEvents[i].second);
			}
		}

		for (size_t i = 0; i < mPeriodic.size(); ++i) {
			const PeriodicMemoryStats::Stats& s = mPeriodic[i];
			std::chrono::duration<double> const diff = s.timestamp - begin;
			linePositionsX.push_back(std::to_string(diff.count()));
			linePositionsY.push_back(std::to_string((s.memPrivateUsage / (1024.0 * 1024))));
		}
	}

	double total() const {
		std::chrono::duration<double> const diff = mEvents.back().first.timestamp - mPeriodic.begin()->timestamp;
		return diff.count();
	}

	const std::vector<std::pair<Stats, std::string>>& events() const {
		return mEvents;
	}

	const std::vector<Stats>& periodics() const {
		return mPeriodic;
	}

private:
	void runner() {
		auto nextStop = std::chrono::high_resolution_clock::now();

		Stats s;
		while (mDoContinue) {
			nextStop += std::chrono::microseconds(mIntervalMicros);

			s.timestamp = std::chrono::high_resolution_clock::now();
			s.memPrivateUsage = getMem();
			mPeriodic.push_back(s);

			if (nextStop < s.timestamp) {
				// we can't keep up!
				nextStop = s.timestamp;
			}
			std::this_thread::sleep_until(nextStop);
		}

		// add one last measurement
		s.timestamp = std::chrono::high_resolution_clock::now();
		s.memPrivateUsage = getMem();
		mPeriodic.push_back(s);
	}

	size_t getMem() {
#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX info;
		info.cb = sizeof(info);
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&info, info.cb)) {
			return info.PrivateUsage;
		}
#else
		std::ifstream file("/proc/self/status");
		std::string const prefix = "VmSize:";

		std::string line;
		while (std::getline(file, line)) {
			if (0 == line.compare(0, prefix.size(), prefix)) {
				// size is in kB
				return std::atoll(line.data() + prefix.size()) * 1024;
			}
		}
#endif
		return 0;
	}

	std::vector<Stats> mPeriodic;
	std::vector<std::pair<Stats, std::string>> mEvents;

	uint64_t mIntervalMicros;
	bool mDoContinue;

	std::thread mThread;
};
