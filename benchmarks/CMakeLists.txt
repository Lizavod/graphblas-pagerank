# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

add_executable(benchmark benchmark.c bench_pagerank.c)

target_include_directories(benchmark PRIVATE ${CMAKE_SOURCE_DIR}/src ${GRAPHBLAS_INCLUDE_DIR} ${LAGRAPH_INCLUDE_DIR})
target_link_libraries(benchmark PRIVATE pagerank_lib ${GRAPHBLAS_LIBRARIES} ${LAGRAPH_LIBRARIES})

target_compile_options(benchmark PRIVATE -O3 -DNDEBUG)
target_link_options(benchmark PRIVATE -fno-sanitize=all)