cmake_minimum_required(VERSION 3.15)
project(parallel_quicksort_parlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Оптимизации
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG")

add_executable(quicksort_bench
    src/main.cpp
)

# Путь к заголовкам parlaylib:
# Вариант 1: parlaylib лежит как подмодуль в external/parlaylib
# target_include_directories(quicksort_bench PRIVATE external/parlaylib/include)

# Вариант 2: parlaylib установлена глобально и лежит, например, в /usr/local/include
# тогда CMAKE_INCLUDE_PATH уже всё знает, и этот include можно не писать или заменить на нужный путь:
# target_include_directories(quicksort_bench PRIVATE /usr/local/include)

# Для простоты – предполагаем, что ты сам пропишешь нужный include-dir. Пока просто:
target_include_directories(quicksort_bench PRIVATE ${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)
target_link_libraries(quicksort_bench PRIVATE Threads::Threads)
