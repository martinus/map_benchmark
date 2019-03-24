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


# Maps I couldn't add
* [QHash](https://github.com/qt/qtbase): It's interface is too different to be easily includeable. e.g. `iterator->first` and `iterator->second` do not exist.
* [rigtorp::HashMap](https://github.com/rigtorp/HashMap): Doesn't have a default constructor


# Reliable Benchmarks

1. Run `lscpu --extended` to find out if you have hyperthreadding. E.g. for me it shows 
   ```
   $ lscpu --extended
   CPU NODE SOCKET CORE L1d:L1i:L2:L3 ONLINE MAXMHZ    MINMHZ
   0   0    0      0    0:0:0:0       yes    4600,0000 800,0000
   1   0    0      1    1:1:1:0       yes    4600,0000 800,0000
   2   0    0      2    2:2:2:0       yes    4600,0000 800,0000
   3   0    0      3    3:3:3:0       yes    4600,0000 800,0000
   4   0    0      4    4:4:4:0       yes    4600,0000 800,0000
   5   0    0      5    5:5:5:0       yes    4600,0000 800,0000
   6   0    0      0    0:0:0:0       yes    4600,0000 800,0000
   7   0    0      1    1:1:1:0       yes    4600,0000 800,0000
   8   0    0      2    2:2:2:0       yes    4600,0000 800,0000
   9   0    0      3    3:3:3:0       yes    4600,0000 800,0000
   10  0    0      4    4:4:4:0       yes    4600,0000 800,0000
   11  0    0      5    5:5:5:0       yes    4600,0000 800,0000
   ```
1. Isolate a CPU with it's hyperthreading companion. I'm isolating CPU 5 and 11.
1. Edit `/etc/default/grub` and change GRUB_CMDLINE_LINUX_DEFAULT so it looks like this:
   ```
   GRUB_CMDLINE_LINUX_DEFAULT="quiet splash isolcpus=5,11 rcu_nocbs=5,11"
   ```
1. Run `sudo update-grub`
1. reboot
1. Edit `bench.rb` so the `taskset -c ...` prefix is correct.
1. Install Python module `perf`, see https://perf.readthedocs.io/en/latest/
1. Run `sudo python3 -m perf system tune`
1. Start the benchmarks: ```../tools/bench.rb |tee ../data/all_new.txt```

Sources:
* https://wiki.ubuntuusers.de/GRUB_2/Konfiguration/
* https://perf.readthedocs.io/en/latest/system.html