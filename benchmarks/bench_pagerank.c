// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include <GraphBLAS.h>
#include <LAGraph.h>
#include "pagerank.h"
#include <stdio.h>

#define NUM_MEASUREMENTS 30

int run_experiment(const char *graph, int num_iterations)
{
    GrB_Matrix A = NULL;
    LAGraph_Graph G = NULL;
    GrB_Vector my_pr = NULL;
    GrB_Vector la_pr = NULL;
    char msg[LAGRAPH_MSG_LEN];
    double damping = 0.85;
    double tol = 0;
    int iters = 0;

    char graph_filepath[128];
    snprintf(graph_filepath, sizeof(graph_filepath), "benchmarks/data/%s.mtx",
             graph);
    FILE *graph_file = fopen(graph_filepath, "r");
    if (!graph_file) {
        fprintf(stderr, "Error: could not open %s\n", graph_filepath);
        return 1;
    }
    if (LAGraph_MMRead(&A, graph_file, msg) != GrB_SUCCESS) {
        fprintf(stderr, "Error: could not read graph %s\n", msg);
        fclose(graph_file);
        return 1;
    }
    fclose(graph_file);

    char my_res_filepath[128];
    char la_res_filepath[128];
    snprintf(my_res_filepath, sizeof(my_res_filepath),
             "benchmarks/results/%s/my_pagerank.csv", graph);
    snprintf(la_res_filepath, sizeof(la_res_filepath),
             "benchmarks/results/%s/lagraph_pagerank.csv", graph);
    FILE *my_results = fopen(my_res_filepath, "w");
    FILE *la_results = fopen(la_res_filepath, "w");

    if (!my_results || !la_results) {
        fprintf(stderr, "Error: could not open results files\n");
        GrB_Matrix_free(&A);
        if (my_results) {
            fclose(my_results);
        }
        if (la_results) {
            fclose(la_results);
        }
        return 1;
    }

    if (LAGraph_New(&G, &A, LAGraph_ADJACENCY_DIRECTED, msg) != GrB_SUCCESS) {
        fprintf(stderr, "Error: could not create graph to LAGraph\n");
        GrB_Matrix_free(&A);
        GrB_Vector_free(&my_pr);
        fclose(my_results);
        fclose(la_results);
        return 1;
    }
    LAGraph_Cached_AT(G, msg);
    LAGraph_Cached_OutDegree(G, msg);

    pagerank(&my_pr, &iters, G->A, damping, tol, num_iterations);
    LAGr_PageRank(&la_pr, &iters, G, damping, tol, num_iterations, msg);

    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
        double t1 = LAGraph_WallClockTime();
        pagerank(&my_pr, &iters, G->A, damping, tol, num_iterations);
        double t2 = LAGraph_WallClockTime();
        if (iters != num_iterations) {
            fprintf(stderr,
                    "pagerank() has not completed all iterations: %d != %d",
                    iters, num_iterations);
        }

        fprintf(my_results, "%10.6f\n", t2 - t1);
        GrB_Vector_free(&my_pr);
    }

    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
        double t1 = LAGraph_WallClockTime();
        LAGr_PageRank(&la_pr, &iters, G, damping, tol, num_iterations, msg);
        double t2 = LAGraph_WallClockTime();
        if (iters != num_iterations) {
            fprintf(
                stderr,
                "LAGr_PageRank() has not completed all iterations: %d != %d",
                iters, num_iterations);
        }
        fprintf(la_results, "%10.6f\n", t2 - t1);
        GrB_Vector_free(&la_pr);
    }

    fclose(my_results);
    fclose(la_results);
    GrB_Vector_free(&my_pr);
    GrB_Vector_free(&la_pr);
    LAGraph_Free((void **)&G, msg);
    return 0;
}