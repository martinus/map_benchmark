cmake_minimum_required(VERSION 2.8.9)
project (map_benchmark)

# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS "-ggdb -O3 -march=native ${CMAKE_CXX_FLAGS}")

file(GLOB SRC_BENCHMARKS "src/benchmarks/*.cpp")
file(GLOB SRC_APP "src/app/*.cpp")

file(GLOB INC_HASHES "src/hashes/*")
file(GLOB INC_MAPS "src/maps/*")

# external code #####################################################################

add_subdirectory("external/robin_hood")
add_subdirectory("external/ska")
add_subdirectory("external/sparsepp")
add_subdirectory("external/tsl")

# absl stuff: see https://github.com/abseil/abseil-cpp/tree/master/CMake
if(MSVC)
    # /wd4005  macro-redefinition
    # /wd4068  unknown pragma
    # /wd4244  conversion from 'type1' to 'type2'
    # /wd4267  conversion from 'size_t' to 'type2'
    # /wd4800  force value to bool 'true' or 'false' (performance warning)
    add_compile_options(/wd4005 /wd4068 /wd4244 /wd4267 /wd4800)
    add_definitions(/DNOMINMAX /DWIN32_LEAN_AND_MEAN=1 /D_CRT_SECURE_NO_WARNINGS)
endif()
add_subdirectory("external/abseil-cpp")

# pthread, required by absl see https://github.com/abseil/abseil-cpp/issues/183#issuecomment-458453010
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# folly 
# install dependencies, see https://github.com/facebook/folly
# sudo apt-get install \
#   g++ \
#   cmake \
#   libboost-all-dev \
#   libevent-dev \
#   libdouble-conversion-dev \
#   libgoogle-glog-dev \
#   libgflags-dev \
#   libiberty-dev \
#   liblz4-dev \
#   liblzma-dev \
#   libsnappy-dev \
#   make \
#   zlib1g-dev \
#   binutils-dev \
#   libjemalloc-dev \
#   libssl-dev \
#   pkg-config
add_subdirectory("external/folly")


# build stuff #####################################################################

# use ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif(CCACHE_PROGRAM)

# I like colors, even with ninja build
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif ()

# create targets for all map - hash pairs
foreach(MAP_DIR ${INC_MAPS})
    get_filename_component(MAP_NAME ${MAP_DIR} NAME_WE)
    foreach(HASH_DIR ${INC_HASHES})
        # executable name: mapname_hashname
        get_filename_component(HASH_NAME ${HASH_DIR} NAME_WE)
        set(EXECUTABLE_NAME "bench_${MAP_NAME}__${HASH_NAME}")

        # collect .cpp files in current hash and map directory
        file(GLOB SRC_MAP_DIR "${MAP_DIR}/*.cpp")
        file(GLOB SRC_HASH_DIR "${HASH_DIR}/*.cpp")

        add_executable(${EXECUTABLE_NAME} ${SRC_APP} ${SRC_BENCHMARKS} ${SRC_MAP_DIR} ${SRC_HASH_DIR})
        target_include_directories(${EXECUTABLE_NAME} PRIVATE "src/app" "external" ${MAP_DIR} ${HASH_DIR} ${FOLLY_DIR})
        
        # absl
        target_link_libraries(${EXECUTABLE_NAME} absl::base absl::flat_hash_map)
        target_link_libraries(${EXECUTABLE_NAME} Threads::Threads)

        # folly
        target_link_libraries(${EXECUTABLE_NAME} folly)

        endforeach(HASH_DIR ${INC_HASHES})
endforeach(MAP_DIR ${INC_MAPS})
