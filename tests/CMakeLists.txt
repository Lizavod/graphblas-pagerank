# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta


set(TEST_FLAGS -Wall -Wextra -Wpedantic -Werror)

set(TEST_NAMES tests_reader tests_pagerank)

add_library(csv_reader STATIC utils/csv_reader.c)
target_include_directories(csv_reader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${GRAPHBLAS_INCLUDE_DIR}
)

foreach(test_name ${TEST_NAMES})
    add_executable(${test_name} ${test_name}.c)
    target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/include ${GRAPHBLAS_INCLUDE_DIR} ${CMOCKA_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/utils)
    target_link_libraries(${test_name} pagerank_lib csv_reader ${CMOCKA_LIBRARIES})
    target_compile_options(${test_name} PRIVATE ${TEST_FLAGS})
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
