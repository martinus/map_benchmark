CXX=ccache g++-8
CXX_FLAGS=-ggdb -O3 -std=c++14
#CXX_FLAGS=-g -std=c++14

maps=\
	robin_hood_flat_map \
	robin_hood_node_map \
	robin_hood_unordered_map \
	std_unordered_map \
	skarupke_flat_hash_map \
	absl_flat_hash_map \
	absl_node_hash_map

memprofile_binaries=$(patsubst %,build/memprofile/%,$(maps))
runtime_binaries=$(patsubst %,build/runtime/%,$(maps))

all: $(memprofile_binaries) $(runtime_binaries)

all_fetch: $(patsubst %,fetch/%,$(maps))

fetch/%: src/maps/$(@F)
	$(MAKE) -C src/maps/$(@F)

clean: 
	rm -f $(memprofile_binaries) $(runtime_binaries)

build/memprofile/%: src/maps/$(@F)
	$(CXX) $(CXX_FLAGS) -DENABLE_MALLOC_HOOK -Isrc/maps/$(@F) -Isrc/app -lm -o build/memprofile/memprofile_$(@F) src/app/*.cpp src/benchmarks/*.cpp -pthread -ldl -L/home/martinus/dev/abseil-cpp/bazel-bin/absl/hash -lhash -lcity -L/home/martinus/dev/abseil-cpp/bazel-bin/absl/container -lraw_hash_set

build/runtime/%: src/maps/$(@F)
	$(CXX) $(CXX_FLAGS) -Isrc/maps/$(@F) -Isrc/app -lm -o build/runtime/$(@F) src/app/*.cpp src/benchmarks/*.cpp -pthread -ldl  -L/home/martinus/dev/abseil-cpp/bazel-bin/absl/hash -lhash -lcity -L/home/martinus/dev/abseil-cpp/bazel-bin/absl/container -lraw_hash_set

# $(eval TESTS=$(shell build/memprofile/std_unordered_map l))
TESTS := RandomFindExisting RandomFindNonExisting

run_memprofile:
	for test in $(TESTS); do \
		for bin in $(memprofile_binaries); do \
			./$$bin f $$test; \
		done \
	done

.PHONY: clean all
