// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "pagerank.h"
#include <GraphBLAS.h>

GrB_Info pagerank(GrB_Vector *centrality, int *iters, const GrB_Matrix A,
                  double damping, double tol, int max_iters)
{

    (void)damping;
    (void)tol;
    (void)max_iters;

    GrB_Index n;
    GrB_Matrix_nrows(&n, A);

    GrB_Vector_new(centrality, GrB_FP64, n);

    if (iters) {
        *iters = 0;
    }
    return GrB_SUCCESS;
}
