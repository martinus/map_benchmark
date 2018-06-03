CXX=g++
CXX_FLAGS=-O2 -std=c++14

binaries=\
	build/std_unordered_map \
	build/std_map \
	build/null_map

all: $(binaries)
	
clean: 
	rm -f $(binaries)

build/%: src/maps/$(@F)
	$(CXX) $(CXX_FLAGS) -Isrc/maps/$(@F) -Isrc/app -lm -o build/$(@F) src/app/main.cpp

.PHONY: clean all
