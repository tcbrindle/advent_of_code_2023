
cmake_minimum_required(VERSION 3.28)
project(aoc2023 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    flux
    GIT_REPOSITORY https://github.com/tcbrindle/flux.git
    GIT_TAG d88fd970d26ec6fc297965ce1dd60b81a5b2def1
)

FetchContent_Declare(
    ctre
    GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
    GIT_TAG v3.9.0
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

FetchContent_Declare(
    unordered_dense
    GIT_REPOSITORY https://github.com/martinus/unordered_dense.git
    GIT_TAG v4.4.0
)

FetchContent_MakeAvailable(flux ctre fmt unordered_dense)

add_library(aoc INTERFACE)
target_sources(
    aoc INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES aoc.hpp)
target_link_libraries(aoc INTERFACE ctre::ctre fmt::fmt flux::flux unordered_dense::unordered_dense)
target_precompile_headers(aoc INTERFACE aoc.hpp)

function(ADD_DAY DATE)
    add_executable(${DATE} ${DATE}/main.cpp)
    target_link_libraries(${DATE} PRIVATE aoc)
endfunction()

add_day(dec01)
add_day(dec02)
add_day(dec03)
add_day(dec04)
add_day(dec05)
add_day(dec06)
add_day(dec07)
add_day(dec08)
add_day(dec09)
add_day(dec10)
add_day(dec11)
add_day(dec12)
add_day(dec13)
add_day(dec14)
add_day(dec15)
add_day(dec16)
add_day(dec17)
add_day(dec18)
add_day(dec19)
add_day(dec20)
add_day(dec21)
add_day(dec22)
#add_day(dec23)
add_day(dec24)

