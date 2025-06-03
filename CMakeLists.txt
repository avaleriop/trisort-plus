cmake_minimum_required(VERSION 3.20)
project(trisort_c VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native")

add_library(trisort STATIC
    src/entropy.c src/hilbert_bucket.c src/cache_block.c
    src/introsmooth.c src/trisort_plus.c src/common.c)

target_include_directories(trisort PUBLIC include)

# Python module
find_package(Python3 COMPONENTS Development REQUIRED)
add_library(trisortc MODULE src/pybind.c)
target_link_libraries(trisortc PRIVATE trisort Python3::Python)
target_include_directories(trisortc PRIVATE include)
set_target_properties(trisortc PROPERTIES PREFIX "" SUFFIX ".so")

# Smoke test
add_executable(smoke tests/smoke.c)
target_link_libraries(smoke PRIVATE trisort)
add_test(NAME smoke COMMAND smoke)
