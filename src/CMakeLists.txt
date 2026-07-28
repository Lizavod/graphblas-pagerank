# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

add_library(pagerank_lib STATIC pagerank.c)

target_include_directories(pagerank_lib PUBLIC ${PROJECT_SOURCE_DIR}/include ${GRAPHBLAS_INCLUDE_DIR})
target_compile_options(pagerank_lib PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(pagerank_lib PUBLIC ${GRAPHBLAS_LIBRARY})