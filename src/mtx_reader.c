// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "mtx_reader.h"
#include <GraphBLAS.h>

#include <GraphBLAS.h>

GrB_Info read_matrix(const char *filename, GrB_Matrix *A)
{

    (void)filename;

    GrB_Matrix_new(A, GrB_FP64, 1, 1);

    return GrB_SUCCESS;
}