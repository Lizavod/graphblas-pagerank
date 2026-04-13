// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "reader.h"
#include <GraphBLAS.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 256

#define PRINT_ERROR(...) (fprintf(stderr, __VA_ARGS__))

static void free_memory(GrB_Index **I, GrB_Index **J, double **X)
{
    free(*I);
    free(*J);
    free(*X);
    *I = NULL;
    *J = NULL;
    *X = NULL;
}

GrB_Info read_matrix(const char *filename, GrB_Matrix *A)
{
    FILE *file = fopen(filename, "r");
    GrB_Info status = GrB_SUCCESS;
    char line[MAX_LINE];
    char *ptr = NULL;
    int num_nodes = 0, num_edges = 0;
    int count = 0;
    uint64_t from = 0, to = 0;
    double val = 1;

    if (!file) {
        PRINT_ERROR("Error: could not open the file %s\n", filename);
        return GrB_PANIC;
    }

    if (fscanf(file, "%d,%d", &num_nodes, &num_edges) != 1) {
        PRINT_ERROR("Error: couldn't read data\n");
        fclose(file);
        return GrB_INVALID_VALUE;
    }

    GrB_Index *I = malloc(num_edges * sizeof(GrB_Index));
    GrB_Index *J = malloc(num_edges * sizeof(GrB_Index));
    double *X = malloc(num_edges * sizeof(double));
    if (!I || !J || !X) {
        PRINT_ERROR("Error: couldn't allocate memory\n");
        free_memory(&I, &J, &X);
        return GrB_OUT_OF_MEMORY;
    }

    while (fgets(line, MAX_LINE, file)) {
        ptr = line;
        if (fscanf(file, "%lu,%lu", &from, &to) != 2) {
            PRINT_ERROR("Error: couldn't read data\n");
            free_memory(&I, &J, &X);
            fclose(file);
            return GrB_INVALID_VALUE;
        }
        I[count] = (GrB_Index)(row - 1);
        J[count] = (GrB_Index)(col - 1);
        X[count] = val;
        count++;
    }

    if (A != NULL) {
        GrB_Matrix_free(A);
    }
    status = GrB_Matrix_new(A, GrB_FP64, num_nodes, num_nodes);
    if (status != GrB_SUCCESS) {
        PRINT_ERROR("Error: failed to create a matrix\n");
        free_memory(&I, &J, &X);
        fclose(file);
        return status;
    }

    status = GrB_Matrix_build_FP64(*A, I, J, X, count, GrB_SECOND_FP64);
    if (status != GrB_SUCCESS) {
        PRINT_ERROR("Error: failed to create a matrix\n");
        GrB_Matrix_free(A);
        free_memory(&I, &J, &X);
        fclose(file);
        return status;
    }

    GrB_Matrix_free(A);
    free_memory(&I, &J, &X);
    fclose(file);
    return status;
}
