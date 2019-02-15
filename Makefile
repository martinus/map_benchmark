CXX=g++-8
CXX_FLAGS=-ggdb -O3 -march=native -std=c++14

#CXX_FLAGS+=-DENABLE_MALLOC_HOOK -pthread
SOURCES=src/benchmarks/*.cpp src/app/*.cpp 
INCLUDES=-Isrc/app
LIBS=-lm -ldl
DEFAULTS=$(CXX) $(CXX_FLAGS) $(INCLUDES) $(SOURCES) $(LIBS)

# standard compiliation is good enough
DEFAULT_MAPS=\
	std_unordered_map_cityhash \
	std_unordered_map_fnv1ahash \
	std_unordered_map_nullhash \
	std_unordered_map_robinhoodhash \
	robin_hood_unordered_flat_map_cityhash \
	robin_hood_unordered_flat_map_fnv1ahash \
	robin_hood_unordered_flat_map_nullhash \
	robin_hood_unordered_flat_map_robinhoodhash \
	robin_hood_unordered_node_map_cityhash \
	robin_hood_unordered_node_map_fnv1ahash \
	robin_hood_unordered_node_map_nullhash \
	robin_hood_unordered_node_map_robinhoodhash \
	ska_bytell_hash_map_cityhash \
	ska_bytell_hash_map_fnv1ahash \
	ska_bytell_hash_map_nullhash \
	ska_bytell_hash_map_robinhoodhash \
	tsl_hopscotch_fnv1ahash \
	tsl_hopscotch_cityhash \
	tsl_hopscotch_nullhash \
	tsl_hopscotch_robinhoodhash \
	tsl_robin_cityhash \
	tsl_robin_fnv1ahash \
	tsl_robin_nullhash \
	tsl_robin_robinhoodhash \
	tsl_sparse_cityhash \
	tsl_sparse_fnv1ahash \
	tsl_sparse_nullhash \
	tsl_sparse_robinhoodhash \

# hand coded targets with special requirements
ABSL_MAPS=\
	absl_flat_hash_map_cityhash \
	absl_flat_hash_map_fnv1ahash \
	absl_flat_hash_map_nullhash \
	absl_flat_hash_map_robinhoodhash \
	absl_node_hash_map_cityhash \
	absl_node_hash_map_fnv1ahash \
	absl_node_hash_map_nullhash \
	absl_node_hash_map_robinhoodhash \


DEFAULT_BINARIES=$(patsubst %,build/%,$(DEFAULT_MAPS))
ABSL_BINARIES=$(patsubst %,build/%,$(ABSL_MAPS))

all: $(DEFAULT_BINARIES) $(ABSL_BINARIES)

build/%:
	$(DEFAULTS) -include src/maps/$(@F).h -o build/bench_$(@F)

build/absl_%:
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -Isrc/maps/absl -include src/maps/$(@F).h $(SOURCES) $(LIBS) -L/home/martinus/dev/abseil-cpp/bazel-bin/absl/hash -lhash -lcity -L/home/martinus/dev/abseil-cpp/bazel-bin/absl/container -lraw_hash_set -o build/bench_$(@F)

clean:
	rm -f build/bench*

.PHONY: all clean