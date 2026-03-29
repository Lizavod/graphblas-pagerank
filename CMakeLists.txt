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

find_path(GRAPHBLAS_INCLUDE_DIR
    NAMES GraphBLAS.h
    PATHS 
        /usr/local/include/suitesparse
        /usr/local/include
        /usr/include/suitesparse
        /usr/include
)

find_library(GRAPHBLAS_LIBRARY
    NAMES graphblas suitesparse-graphblas GraphBLAS
    PATHS 
        /usr/local/lib/x86_64-linux-gnu
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib
)

if(NOT GRAPHBLAS_INCLUDE_DIR OR NOT GRAPHBLAS_LIBRARY)
    message(FATAL_ERROR "GraphBLAS not found")
endif()

find_package(cmocka REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

install(FILES 
    include/pagerank.h
    include/mtx_reader.h
    DESTINATION include/pagerank
)

install(TARGETS pagerank_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)