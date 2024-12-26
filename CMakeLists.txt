cmake_minimum_required(VERSION 3.14)
project(mutant)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")


include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/duckdb/src/include)

link_directories(${CMAKE_SOURCE_DIR}/third_party/duckdb/build/release/src)

add_executable(mutant
    main.cpp
    src/cli/CLI.cpp
    src/core/Mutator.cpp
    src/core/MutantTree.cpp
    src/core/TreeNode.cpp
    src/core/Comparator.cpp
)

# Link DuckDB library
target_link_libraries(mutant PRIVATE duckdb)
