# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

cmake_minimum_required(VERSION 3.12)
project(pagerank_graphblas LANGUAGES C)

set(CMAKE_C_STANDARD 99)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fsanitize=address -fsanitize=undefined -fsanitize=leak")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined -fsanitize=leak")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

find_package(GraphBLAS REQUIRED)
find_package(cmocka REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

install(FILES 
    include/pagerank.h
    DESTINATION include/pagerank
)

install(TARGETS pagerank_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)