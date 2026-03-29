// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "mtx_reader.h"
#include "pagerank.h"
#include <setjmp.h>
#include <cmocka.h>

static void test_stub(void **state)
{
    (void)state;
    GrB_Matrix A;
    read_matrix("data/single_node.mtx", &A);
    GrB_Vector rank;
    assert_int_equal(pagerank(&rank, NULL, A, 0.85, 1e-6, 100), GrB_SUCCESS);
    GrB_Vector_free(&rank);
    GrB_Matrix_free(&A);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_stub),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}