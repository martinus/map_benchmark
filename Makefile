CXX=g++-8
CXX_FLAGS=-ggdb -O3 -march=native -std=c++14

DEFAULTS=$(CXX) $(CXX_FLAGS) -DENABLE_MALLOC_HOOK -Isrc/app src/benchmarks/*.cpp src/app/*.cpp -lm -pthread -ldl

# standard compiliation is good enough
DEFAULT_MAPS=\
	std_unordered_map_fnv1ahash \
	std_unordered_map_nullhash \
	std_unordered_map_robinhoodhash \
	robin_hood_unordered_flat_map_fnv1ahash \
	robin_hood_unordered_flat_map_nullhash \
	robin_hood_unordered_flat_map_robinhoodhash \
	tsl_robin_hash_fnv1ahash \
	tsl_robin_hash_nullhash \
	tsl_robin_hash_robinhoodhash \

# hand coded targets with special requirements
SPECIAL_TARGETS=\
	build/absl_flat_hash_map

DEFAULT_BINARIES=$(patsubst %,build/%,$(DEFAULT_MAPS))

all: $(DEFAULT_BINARIES) $(SPECIAL_TARGETS)

build/%:
	$(DEFAULTS) -include src/maps/$(@F).h -o build/bench_$(@F)

build/absl_flat_hash_map:
	$(DEFAULTS) -include src/maps/absl_flat_hash_map.h $(SOURCES) -Isrc/maps/absl -pthread -ldl -L/home/martinus/dev/abseil-cpp/bazel-bin/absl/hash -lhash -lcity -L/home/martinus/dev/abseil-cpp/bazel-bin/absl/container -lraw_hash_set -o build/bench_absl_flat_hash_map

clean:
	rm -f build/bench*

.PHONY: all clean