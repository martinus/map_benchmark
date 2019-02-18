# map_benchmark
Comprehensive benchmarks of C++ maps

# building

Install dependencies for folly::F14, see https://github.com/facebook/folly/#ubuntu-1604-lts

    git clone --recurse-submodules https://github.com/martinus/map_benchmark.git
    cd map_benchmark/build
    cmake -DCMAKE_CXX_COMPILER="g++-8" -G Ninja ..
    ninja

# Add a new Hashmap

1. In `external`, add a submodule:
   ```
   git submodule add -b master https://github.com/rigtorp/HashMap.git rigtorp__HashMap
   ```
2. Create a directory in `src/map/` with a file `Hash.h`. See the others for example.