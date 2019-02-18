# map_benchmark
Comprehensive benchmarks of C++ maps

# building

Install dependencies for folly::F14, see https://github.com/facebook/folly/#ubuntu-1604-lts

    git clone --recurse-submodules https://github.com/martinus/map_benchmark.git
    cd map_benchmark/build
    cmake -DCMAKE_CXX_COMPILER="g++-8" -G Ninja ..
    ninja

