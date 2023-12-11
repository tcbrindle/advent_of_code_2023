
cmake_minimum_required(VERSION 3.16)
project(aoc2023 CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(dec01 dec01/main.cpp)
target_precompile_headers(dec01 PRIVATE aoc.hpp)

add_executable(dec02 dec02/main.cpp)
target_precompile_headers(dec02 PRIVATE aoc.hpp)

add_executable(dec03 dec03/main.cpp)
target_precompile_headers(dec03 PRIVATE aoc.hpp)

add_executable(dec04 dec04/main.cpp)
target_precompile_headers(dec04 PRIVATE aoc.hpp)

add_executable(dec05 dec05/main.cpp)
target_precompile_headers(dec05 PRIVATE aoc.hpp)

add_executable(dec06 dec06/main.cpp)
target_precompile_headers(dec06 PRIVATE aoc.hpp)

add_executable(dec07 dec07/main.cpp)
target_precompile_headers(dec07 PRIVATE aoc.hpp)

add_executable(dec08 dec08/main.cpp)
target_precompile_headers(dec08 PRIVATE aoc.hpp)

add_executable(dec09 dec09/main.cpp)
target_precompile_headers(dec09 PRIVATE aoc.hpp)

add_executable(dec10 dec10/main.cpp)
target_precompile_headers(dec10 PRIVATE aoc.hpp)

add_executable(dec11 dec11/main.cpp)
target_precompile_headers(dec11 PRIVATE aoc.hpp)
