// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "csv_reader.h"
#include <GraphBLAS.h>
#include <setjmp.h>
#include <cmocka.h>

typedef struct {
    const char *filename;
    GrB_Matrix A;
    GrB_Index expected_num_nodes;
    GrB_Index expected_num_edges;
    struct {
        GrB_Index row;
        GrB_Index col;
    } checks[10];
    int num_checks;
} test_data_t;

test_data_t *current_test_data = NULL;

static int setup_matrix(void **state)
{
    (void)state;
    if (read_matrix(current_test_data->filename, &current_test_data->A) !=
        GrB_SUCCESS) {
        return -1;
    }
    return 0;
}

static int teardown_matrix(void **state)
{
    (void)state;
    GrB_Matrix_free(&current_test_data->A);
    return 0;
}

static void test_matrix_size(void **state)
{
    (void)state;
    GrB_Index size;
    GrB_Matrix_nrows(&size, current_test_data->A);
    assert_int_equal(size, current_test_data->expected_num_nodes);
}

static void test_num_edges(void **state)
{
    (void)state;
    GrB_Index num_edges;
    GrB_Matrix_nvals(&num_edges, current_test_data->A);
    assert_int_equal(num_edges, current_test_data->expected_num_edges);
}

static void test_elements(void **state)
{
    (void)state;
    double val = 0.0;
    for (int i = 0; i < current_test_data->num_checks; i++) {
        GrB_Matrix_extractElement_FP64(&val, current_test_data->A,
                                       current_test_data->checks[i].row,
                                       current_test_data->checks[i].col);
        assert_double_equal(val, 1.0, 1e-6);
    }
}

int main(void)
{
    GrB_init(GrB_NONBLOCKING);
    int result = 0;

    test_data_t graph_one_node = {
        .filename = "data/graph_one_node/graph.csv",
        .expected_num_nodes = 1,
        .expected_num_edges = 1,
        .checks = {{0, 0}},
        .num_checks = 1,
    };

    test_data_t graph_two_node = {
        .filename = "data/graph_two_node/graph.csv",
        .expected_num_nodes = 2,
        .expected_num_edges = 1,
        .checks = {{0, 1}},
        .num_checks = 1,
    };
    test_data_t graph_chain = {
        .filename = "data/graph_chain/graph.csv",
        .expected_num_nodes = 4,
        .expected_num_edges = 3,
        .checks = {{0, 1}, {1, 2}, {2, 3}},
        .num_checks = 3,
    };
    test_data_t graph_star = {
        .filename = "data/graph_star/graph.csv",
        .expected_num_nodes = 4,
        .expected_num_edges = 3,
        .checks = {{0, 1}, {0, 2}, {0, 3}},
        .num_checks = 3,
    };

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_matrix_size),
        cmocka_unit_test(test_num_edges),
        cmocka_unit_test(test_elements),
    };

    current_test_data = &graph_one_node;
    result |= cmocka_run_group_tests_name("graph_one_node", tests, setup_matrix,
                                          teardown_matrix);
    current_test_data = &graph_two_node;
    result |= cmocka_run_group_tests_name("graph_two_node", tests, setup_matrix,
                                          teardown_matrix);
    current_test_data = &graph_chain;
    result |= cmocka_run_group_tests_name("graph_chain", tests, setup_matrix,
                                          teardown_matrix);
    current_test_data = &graph_star;
    result |= cmocka_run_group_tests_name("graph_star", tests, setup_matrix,
                                          teardown_matrix);
    GrB_finalize();
    return result;
}
