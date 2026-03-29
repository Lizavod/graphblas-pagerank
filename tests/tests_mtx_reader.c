// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "mtx_reader.h"
#include <setjmp.h>
#include <cmocka.h>

static void test_stub(void **state)
{
    (void)state;
    GrB_Matrix A;
    assert_int_equal(read_matrix("data/single_node.mtx", &A), GrB_SUCCESS);
    GrB_Matrix_free(&A);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_stub),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}