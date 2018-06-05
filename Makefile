CXX=g++
CXX_FLAGS=-O2 -std=c++14

maps=\
	std_unordered_map \
	std_map \
	null_map \
	skarupke_flat_hash_map

binaries=$(patsubst %,build/%,$(maps))

all: $(binaries)

all_fetch: $(patsubst %,fetch/%,$(maps))

fetch/%: src/maps/$(@F)
	$(MAKE) -C src/maps/$(@F)

clean: 
	rm -f $(binaries)

build/%: src/maps/$(@F)
	$(CXX) $(CXX_FLAGS) -Isrc/maps/$(@F) -Isrc/app -lm -o build/$(@F) src/app/main.cpp src/benchmarks/*.cpp

.PHONY: clean all
