cmake_minimum_required(VERSION 3.15)
project(parallel_quicksort_parlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG")

add_executable(quicksort_bench
    src/main.cpp
)

target_include_directories(quicksort_bench PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/parlaylib/include
)

find_package(Threads REQUIRED)
target_link_libraries(quicksort_bench PRIVATE Threads::Threads)
