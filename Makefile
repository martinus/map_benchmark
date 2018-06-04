CXX=g++
CXX_FLAGS=-O2 -std=c++14

maps=\
	std_unordered_map \
	std_map \
	null_map \
	skarupke_flat_hash_map



all: $(binaries)


fetch/:
	$(MAKE) -C src/maps/skarupke_flat_hash_map

clean: 
	rm -f $(binaries)

build/%: src/maps/$(@F)
	$(CXX) $(CXX_FLAGS) -Isrc/maps/$(@F) -Isrc/app -lm -o build/$(@F) src/app/main.cpp src/benchmarks/*.cpp

.PHONY: clean all
