CXX=ccache g++
CXX_FLAGS=-g -O2 -std=c++14
#CXX_FLAGS=-g -std=c++14

maps=\
	robin_hood_map \
	std_unordered_map \
	std_map \
	null_map \
	skarupke_flat_hash_map

memprofile_binaries=$(patsubst %,build/memprofile/%,$(maps))
runtime_binaries=$(patsubst %,build/runtime/%,$(maps))

all: $(memprofile_binaries) $(runtime_binaries)

all_fetch: $(patsubst %,fetch/%,$(maps))

fetch/%: src/maps/$(@F)
	$(MAKE) -C src/maps/$(@F)

clean: 
	rm -f $(memprofile_binaries) $(runtime_binaries)

build/memprofile/%: src/maps/$(@F)
	$(CXX) $(CXX_FLAGS) -DENABLE_MALLOC_HOOK -Isrc/maps/$(@F) -Isrc/app -lm -o build/memprofile/$(@F) src/app/*.cpp src/benchmarks/*.cpp -pthread -ldl

build/runtime/%: src/maps/$(@F)
	$(CXX) $(CXX_FLAGS) -Isrc/maps/$(@F) -Isrc/app -lm -o build/runtime/$(@F) src/app/*.cpp src/benchmarks/*.cpp -pthread -ldl

.PHONY: clean all
