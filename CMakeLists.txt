
cmake_minimum_required(VERSION 3.16)
project(aoc2023 CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(dec01 dec01/main.cpp)
target_precompile_headers(dec01 PRIVATE aoc.hpp)
