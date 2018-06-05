#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

// A rudimentary profiler
class Profiler {
public:

static size_t get_mem() {
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
    

};