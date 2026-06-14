// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include <GraphBLAS.h>
#include "pagerank.h"

static void rdiv(void *out, const void *in1, const void *in2)
{
    double val1 = *(double *)in1;
    double val2 = *(double *)in2;
    *(double *)out = val1 / val2;
}

GrB_Info pagerank(GrB_Vector *pagerank_scores, int *iters, const GrB_Matrix A,
                  double damping, double tol, int max_iters)
{
    GrB_Matrix M;
    GrB_Matrix inv_degree_diag;

    GrB_Index num_nodes;
    GrB_Scalar scalar_initial;
    GrB_Scalar scalar_zero;
    GrB_Scalar scalar_one;
    GrB_Scalar scalar_dangling_mass;
    GrB_Scalar scalar_error;
    GrB_Scalar scalar_damping;
    GrB_Scalar scalar_ridist;

    GrB_Vector pagerank_prev;
    GrB_Vector out_degree;
    GrB_Vector inv_degree;
    GrB_Vector not_dangling;
    GrB_Vector is_dangling;
    GrB_Vector dangling_scores;
    GrB_Vector difference;

    GrB_Matrix_nrows(&num_nodes, A);
    if (pagerank_scores != NULL && *pagerank_scores != NULL) {
        GrB_Vector_free(pagerank_scores);
    }
    if (num_nodes == 0) {
        return GrB_SUCCESS;
    }

    double initial = 1.0 / num_nodes;
    double current_error = 0.0;
    double redistributed_mass = 0.0;
    double dangling_mass = 0.0;

    GrB_BinaryOp rdiv_op;
    GrB_BinaryOp_new(&rdiv_op, rdiv, GrB_FP64, GrB_FP64, GrB_FP64);

    GrB_Matrix_new(&M, GrB_FP64, num_nodes, num_nodes);

    GrB_Vector_new(pagerank_scores, GrB_FP64, num_nodes);
    GrB_Vector_new(&pagerank_prev, GrB_FP64, num_nodes);
    GrB_Vector_new(&out_degree, GrB_FP64, num_nodes);
    GrB_Vector_new(&inv_degree, GrB_FP64, num_nodes);
    GrB_Vector_new(&not_dangling, GrB_BOOL, num_nodes);
    GrB_Vector_new(&is_dangling, GrB_BOOL, num_nodes);
    GrB_Vector_new(&dangling_scores, GrB_FP64, num_nodes);
    GrB_Vector_new(&difference, GrB_FP64, num_nodes);

    GrB_Scalar_new(&scalar_initial, GrB_FP64);
    GrB_Scalar_new(&scalar_zero, GrB_FP64);
    GrB_Scalar_new(&scalar_one, GrB_FP64);
    GrB_Scalar_new(&scalar_dangling_mass, GrB_FP64);
    GrB_Scalar_new(&scalar_error, GrB_FP64);
    GrB_Scalar_new(&scalar_damping, GrB_FP64);
    GrB_Scalar_new(&scalar_ridist, GrB_FP64);

    GrB_Scalar_setElement_FP64(scalar_initial, initial);
    GrB_Scalar_setElement_FP64(scalar_zero, 0.0);
    GrB_Scalar_setElement_FP64(scalar_one, 1.0);
    GrB_Scalar_setElement_FP64(scalar_damping, damping);

    GxB_Vector_subassign_Scalar(*pagerank_scores, NULL, NULL, scalar_initial,
                                GrB_ALL, num_nodes, NULL);
    GrB_Matrix_reduce_Monoid(out_degree, NULL, NULL, GrB_PLUS_MONOID_FP64, A,
                             NULL);

    GrB_Vector_select_Scalar(not_dangling, NULL, NULL, GrB_VALUENE_FP64,
                             out_degree, scalar_zero, NULL);
    GxB_Vector_subassign_Scalar(is_dangling, NULL, NULL, scalar_one, GrB_ALL,
                                num_nodes, NULL);
    GrB_Vector_select_Scalar(is_dangling, not_dangling, NULL, GrB_VALUEEQ_FP64,
                             not_dangling, scalar_zero, NULL);

    GrB_Vector_apply_BinaryOp1st_Scalar(inv_degree, not_dangling, NULL, rdiv_op,
                                        scalar_one, out_degree, NULL);
    GrB_Matrix_diag(&inv_degree_diag, inv_degree, 0);

    GrB_mxm(M, NULL, NULL, GrB_PLUS_TIMES_SEMIRING_FP64, inv_degree_diag, A,
            NULL);

    GrB_Matrix_free(&inv_degree_diag);
    GrB_Vector_free(&inv_degree);
    GrB_Vector_free(&out_degree);
    GrB_Vector_free(&not_dangling);
    GrB_Scalar_free(&scalar_initial);
    GrB_Scalar_free(&scalar_zero);
    GrB_Scalar_free(&scalar_one);

    for (int i = 0; i < max_iters; i++) {
        GrB_Vector_assign(dangling_scores, is_dangling, NULL, *pagerank_scores,
                          GrB_ALL, num_nodes, NULL);

        GrB_Vector_reduce_Monoid_Scalar(scalar_dangling_mass, NULL,
                                        GrB_PLUS_MONOID_FP64, dangling_scores,
                                        NULL);

        GrB_Scalar_extractElement_FP64(&dangling_mass, scalar_dangling_mass);
        redistributed_mass =
            (damping * dangling_mass + (1.0 - damping)) / num_nodes;

        GrB_Scalar_setElement_FP64(scalar_ridist, redistributed_mass);

        GrB_mxv(pagerank_prev, NULL, NULL, GrB_PLUS_TIMES_SEMIRING_FP64, M,
                *pagerank_scores, GrB_DESC_T0);
        GrB_Vector_apply_BinaryOp1st_Scalar(pagerank_prev, NULL, NULL,
                                            GrB_TIMES_FP64, scalar_damping,
                                            pagerank_prev, NULL);

        GxB_Vector_subassign_Scalar(pagerank_prev, NULL, GrB_PLUS_FP64,
                                    scalar_ridist, GrB_ALL, num_nodes, NULL);

        GrB_Vector_eWiseMult_BinaryOp(difference, NULL, NULL, GrB_MINUS_FP64,
                                      *pagerank_scores, pagerank_prev, NULL);
        GrB_Vector_apply(difference, NULL, NULL, GrB_ABS_FP64, difference,
                         NULL);
        GrB_Vector_reduce_Monoid_Scalar(scalar_error, NULL,
                                        GrB_PLUS_MONOID_FP64, difference, NULL);
        GrB_Scalar_extractElement_FP64(&current_error, scalar_error);
        *iters = i;
        if (current_error < tol) {
            break;
        }

        GrB_Vector temp = *pagerank_scores;
        *pagerank_scores = pagerank_prev;
        pagerank_prev = temp;
    }

    GrB_Matrix_free(&M);
    GrB_Vector_free(&pagerank_prev);
    GrB_Vector_free(&is_dangling);
    GrB_Vector_free(&dangling_scores);
    GrB_Vector_free(&difference);
    GrB_Scalar_free(&scalar_dangling_mass);
    GrB_Scalar_free(&scalar_error);
    GrB_Scalar_free(&scalar_damping);
    GrB_Scalar_free(&scalar_ridist);
    GrB_BinaryOp_free(&rdiv_op);

    return GrB_SUCCESS;
}
