CXX=g++-8
CXX_FLAGS=-ggdb -O3 -march=native -std=c++14

#CXX_FLAGS+=-DENABLE_MALLOC_HOOK -pthread
SOURCES=src/benchmarks/*.cpp src/app/*.cpp 
INCLUDES=-Isrc/app
LIBS=-lm -ldl
DEFAULTS=$(CXX) $(CXX_FLAGS) $(INCLUDES) $(SOURCES) $(LIBS)

# standard compiliation is good enough
DEFAULT_MAPS=\
	robin_hood_unordered_flat_map \
	robin_hood_unordered_node_map \
	ska_bytell_hash_map \
	std_unordered_map \
	tsl_hopscotch_map \
	tsl_robin_map \
	tsl_sparse_map \

# hand coded targets with special requirements
ABSL_MAPS=\
	absl_flat_hash_map \
	absl_node_hash_map \

HASHES=\
	CityHash \
	FNV1a \
	NullHash \
	robin_hood_hash \

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