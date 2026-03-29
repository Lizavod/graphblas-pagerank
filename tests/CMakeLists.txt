# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta


set(TEST_FLAGS -Wall -Wextra -Wpedantic -Werror)

set(TEST_NAMES tests_mtx_reader tests_pagerank)

foreach(test_name ${TEST_NAMES})
    add_executable(${test_name} ${test_name}.c)
    target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/include ${GRAPHBLAS_INCLUDE_DIR} ${CMOCKA_INCLUDE_DIRS})
    target_link_libraries(${test_name} pagerank_lib ${CMOCKA_LIBRARIES})
    target_compile_options(${test_name} PRIVATE ${TEST_FLAGS})
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
