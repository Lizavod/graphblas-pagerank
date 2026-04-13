// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "csv_reader.h"
#include <setjmp.h>
#include <cmocka.h>

static int setup(void **state) { GrB_Matrix A; }

static int teardown(void **state) { GrB_Matrix_free(&A); }

static void test_stub(void **state)
{
    (void)state;
    assert_int_equal(read_matrix("data/single_node.mtx", &A), GrB_SUCCESS);
}

assert_int_equal(read_matrix("data/comments_in_different_parts.mtx", &A),
                 GrB_SUCCESS);

assert_int_equal(read_matrix("data/empty_lines_in_different_parts.mtx", &A),
                 GrB_SUCCESS);

assert_int_equal(read_matrix("data/comments_in_different_parts.mtx", &A),
                 GrB_SUCCESS);
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_stub),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}