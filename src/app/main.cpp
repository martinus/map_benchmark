#include "bench.h"

int main(int, char**) {
    for (auto& nameFn : BenchRegister::nameToFn()) {
        std::cout << nameFn.first << std::endl;
        std::vector<double> times;
        for (size_t i=0; i<5; ++i) {
            Bench bench;
            nameFn.second(bench);
            times.push_back(bench.runtimeSeconds());
            std::cout << "\t" << bench.str() << std::endl;
        }
        std::sort(times.begin(), times.end());

        // TODO write nice JSON data
        for (auto const& t : times) {
            std::cout << "\t\t" << t << std::endl;
        }
    }
}


