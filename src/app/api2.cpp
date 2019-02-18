#include <chrono>
#include <initializer_list>
#include <iostream>

#include <sfc64.h>
#include <unordered_map>

class MyBench {
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

BENCHMARK("insert, clear, reinsert, remove") {
    sfc64 rng(123);
    std::unordered_map<int, int> map;
    bench({"insert 100 random integers", "fsad", "HashName"}, 23423432, [&]() {
        for (size_t n = 0; n < 100'000'000; ++n) {
            map[rng()];
        }
        return map.size();
    });
}