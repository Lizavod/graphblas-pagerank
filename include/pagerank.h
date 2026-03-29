// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#ifndef PAGERANK_H
#define PAGERANK_H

#include <GraphBLAS.h>

GrB_Info pagerank(GrB_Vector *centrality, int *iters, const GrB_Matrix A,
                  double damping, double tol, int max_iters);

#endif