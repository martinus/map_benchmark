#pragma once

#include "MallocHook.h"

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
		size_t mallocBytes;

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

	struct TimeBytes {
		std::string time;
		std::string bytes;
		std::string mallocBytes;
	};
	struct TimeBytesEvent {
		std::string time;
		std::string bytes;
		std::string mallocBytes;
		std::string event;
	};
	struct Plotly {
		std::vector<TimeBytes> periodics;
		std::vector<TimeBytesEvent> events;
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

	Plotly plotly() const {
		Plotly data;

		auto begin = mPeriodic[0].timestamp;
		if (!mEvents.empty()) {
			begin = (std::min)(begin, mEvents[0].first.timestamp);

			for (size_t i = 0; i < mEvents.size(); ++i) {
				const PeriodicMemoryStats::Stats& s = mEvents[i].first;
				std::chrono::duration<double> const diff = s.timestamp - begin;

				TimeBytesEvent tbe;
				tbe.time = std::to_string(diff.count());
				tbe.bytes = std::to_string((s.memPrivateUsage / (1024.0 * 1024)));
				tbe.mallocBytes = std::to_string(s.mallocBytes / (1024.0 * 1024));
				tbe.event = mEvents[i].second;
				data.events.push_back(tbe);
			}
		}

		for (size_t i = 0; i < mPeriodic.size(); ++i) {
			const PeriodicMemoryStats::Stats& s = mPeriodic[i];
			std::chrono::duration<double> const diff = s.timestamp - begin;

			TimeBytes tb;
			tb.time = std::to_string(diff.count());
			tb.bytes = std::to_string((s.memPrivateUsage / (1024.0 * 1024)));
			tb.mallocBytes = std::to_string(s.mallocBytes / (1024.0 * 1024));
			data.periodics.push_back(tb);
		}

		return data;
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

	static size_t getMem() {
#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX info;
		info.cb = sizeof(info);
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&info, info.cb)) {
			return info.PrivateUsage;
		}
#else
		std::ifstream file("/proc/self/statm");
		std::string line;
		std::getline(file, line);
		// skip VmSize, read resident memory (second value, see man proc)
		return std::atoll(line.data() + line.find(' ') + 1) * 1024;
		// return std::atoll(line.data()) * 1024;
#endif
		return 0;
	}

private:
	void runner() {
		auto nextStop = std::chrono::high_resolution_clock::now();

		Stats s;
		while (mDoContinue) {
			nextStop += std::chrono::microseconds(mIntervalMicros);

			s.timestamp = std::chrono::high_resolution_clock::now();
			s.memPrivateUsage = getMem();
			s.mallocBytes = sAllocatedMem.load();
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
		s.mallocBytes = sAllocatedMem.load();
		mPeriodic.push_back(s);
	}

	std::vector<Stats> mPeriodic;
	std::vector<std::pair<Stats, std::string>> mEvents;

	uint64_t mIntervalMicros;
	bool mDoContinue;

	std::thread mThread;
};
