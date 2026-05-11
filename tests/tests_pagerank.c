// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "csv_reader.h"
#include "pagerank.h"
#include <GraphBLAS.h>
#include <setjmp.h>
#include <cmocka.h>

typedef struct {
    const char *graph_path;
    const char *expected_path;
    GrB_Vector pr;
    int iters;
    GrB_Index vector_size;
    GrB_Vector expected;
} test_data_t;

static test_data_t *current_test_data = NULL;

static GrB_Vector read_expected_vector(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    GrB_Index n;
    double val;

    if (fscanf(file, "%zu", &n) != 1) {
        fclose(file);
        return NULL;
    }

    GrB_Vector expected;
    GrB_Vector_new(&expected, GrB_FP64, n);

    for (GrB_Index i = 0; i < n; i++) {
        if (fscanf(file, "%lf", &val) != 1) {
            GrB_Vector_free(&expected);
            fclose(file);
            return NULL;
        }
        GrB_Vector_setElement_FP64(expected, val, i);
    }

    fclose(file);
    return expected;
}

static double sum_vector_element(GrB_Vector vector, GrB_Index n)
{
    double sum = 0.0;
    double val = 0.0;

    for (GrB_Index i = 0; i < n; i++) {
        GrB_Vector_extractElement_FP64(&val, vector, i);
        sum += val;
    }

    return sum;
}

static double max_difference(GrB_Vector vector_first, GrB_Vector vector_second,
                             GrB_Index n1)
{

    GrB_Index n2;
    double max_diff = 0.0;
    double diff = 0.0;
    double val_first = 0.0;
    double val_second = 0.0;

    GrB_Vector_size(&n2, vector_second);

    if (n1 != n2) {
        return -1.0;
    }

    for (GrB_Index i = 0; i < n1; i++) {
        GrB_Vector_extractElement_FP64(&val_first, vector_first, i);
        GrB_Vector_extractElement_FP64(&val_second, vector_second, i);
        diff = fabs(val_first - val_second);
        if (diff > max_diff) {
            max_diff = diff;
        }
    }

    return max_diff;
}

static int setup_group(void **state)
{
    (void)state;
    GrB_Matrix A;
    GrB_Info info;

    info = read_matrix(current_test_data->graph_path, &A);
    if (info != GrB_SUCCESS) {
        return -1;
    }

    current_test_data->expected =
        read_expected_vector(current_test_data->expected_path);
    if (current_test_data->expected == NULL) {
        GrB_Matrix_free(&A);
        return -1;
    }

    info = pagerank(&current_test_data->pr, &current_test_data->iters, A, 0.85,
                    1e-6, 100);
    if (info != GrB_SUCCESS) {
        GrB_Matrix_free(&A);
        return -1;
    }
    GrB_Vector_size(&current_test_data->vector_size, current_test_data->pr);
    GrB_Matrix_free(&A);

    return 0;
}

static int teardown_group(void **state)
{
    (void)state;

    GrB_Vector_free(&current_test_data->pr);
    current_test_data = NULL;

    return 0;
}

static void test_sum_to_one(void **state)
{
    (void)state;
    double sum = sum_vector_element(current_test_data->pr,
                                    current_test_data->vector_size);
    assert_double_equal(sum, 1.0, 1e-6);
}

static void test_equal_to_expected(void **state)
{
    (void)state;
    double diff =
        max_difference(current_test_data->pr, current_test_data->expected,
                       current_test_data->vector_size);
    assert_true(diff < 0.25);
}

int main(void)
{
    GrB_init(GrB_NONBLOCKING);
    int result = 0;

    test_data_t graph_one_node = {.graph_path = "data/graph_one_node/graph.csv",
                                  .expected_path =
                                      "data/graph_one_node/expected.txt"};

    test_data_t graph_two_node = {.graph_path = "data/graph_two_node/graph.csv",
                                  .expected_path =
                                      "data/graph_two_node/expected.txt"};

    test_data_t graph_chain = {.graph_path = "data/graph_chain/graph.csv",
                               .expected_path =
                                   "data/graph_chain/expected.txt"};

    test_data_t graph_complete = {.graph_path = "data/graph_complete/graph.csv",
                                  .expected_path =
                                      "data/graph_complete/expected.txt"};

    test_data_t graph_dangling = {.graph_path = "data/graph_dangling/graph.csv",
                                  .expected_path =
                                      "data/graph_dangling/expected.txt"};

    test_data_t graph_star = {.graph_path = "data/graph_star/graph.csv",
                              .expected_path = "data/graph_star/expected.txt"};

    test_data_t graph_cycle = {.graph_path = "data/graph_cycle/graph.csv",
                               .expected_path =
                                   "data/graph_cycle/expected.txt"};

    test_data_t graph_disconnected = {
        .graph_path = "data/graph_disconnected/graph.csv",
        .expected_path = "data/graph_disconnected/expected.txt"};

    test_data_t graph_empty = {.graph_path = "data/graph_empty/graph.csv",
                               .expected_path =
                                   "data/graph_empty/expected.txt"};

    test_data_t graph_self_loop = {
        .graph_path = "data/graph_self_loop/graph.csv",
        .expected_path = "data/graph_self_loop/expected.txt"};

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sum_to_one),
        cmocka_unit_test(test_equal_to_expected),
    };

    current_test_data = &graph_one_node;
    result |= cmocka_run_group_tests_name("graph_one_node", tests, setup_group,
                                          teardown_group);
    current_test_data = &graph_two_node;
    result |= cmocka_run_group_tests_name("graph_two_node", tests, setup_group,
                                          teardown_group);
    current_test_data = &graph_chain;
    result |= cmocka_run_group_tests_name("graph_chain", tests, setup_group,
                                          teardown_group);
    current_test_data = &graph_complete;
    result |= cmocka_run_group_tests_name("graph_complete", tests, setup_group,
                                          teardown_group);
    current_test_data = &graph_dangling;
    result |= cmocka_run_group_tests_name("graph_dangling", tests, setup_group,
                                          teardown_group);
    current_test_data = &graph_star;
    result |= cmocka_run_group_tests_name("graph_star", tests, setup_group,
                                          teardown_group);
    current_test_data = &graph_cycle;
    result |= cmocka_run_group_tests_name("graph_cycle", tests, setup_group,
                                          teardown_group);
    current_test_data = &graph_disconnected;
    result |= cmocka_run_group_tests_name("graph_disconnected", tests,
                                          setup_group, teardown_group);
    current_test_data = &graph_empty;
    result |= cmocka_run_group_tests_name("graph_empty", tests, setup_group,
                                          teardown_group);
    current_test_data = &graph_self_loop;
    result |= cmocka_run_group_tests_name("graph_self_loop", tests, setup_group,
                                          teardown_group);

    GrB_finalize();
    return result;
}
