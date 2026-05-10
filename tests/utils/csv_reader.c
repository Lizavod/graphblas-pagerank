// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "csv_reader.h"
#include <GraphBLAS.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 256

#define PRINT_ERROR(...) (fprintf(stderr, __VA_ARGS__))

static void free_memory(GrB_Index **I, GrB_Index **J)
{
    free(*I);
    free(*J);
    *I = NULL;
    *J = NULL;
}

GrB_Info read_matrix(const char *filename, GrB_Matrix *A)
{
    FILE *file = fopen(filename, "r");
    GrB_Info status = GrB_SUCCESS;
    char line[MAX_LINE];
    GrB_Index num_nodes = 0, num_edges = 0;
    int count = 0;
    GrB_Index from = 0, to = 0;
    GrB_Scalar val;
    GrB_Scalar_new(&val, GrB_FP64);
    GrB_Scalar_setElement_FP64(val, 1.0);

    if (!file) {
        PRINT_ERROR("Error: could not open the file %s\n", filename);
        return GrB_PANIC;
    }

    if (fscanf(file, "%lu,%lu", &num_nodes, &num_edges) != 2) {
        PRINT_ERROR("Error: couldn't read data\n");
        fclose(file);
        return GrB_INVALID_VALUE;
    }

    GrB_Index *I = malloc(num_edges * sizeof(GrB_Index));
    GrB_Index *J = malloc(num_edges * sizeof(GrB_Index));
    if (!I || !J) {
        PRINT_ERROR("Error: couldn't allocate memory\n");
        free_memory(&I, &J);
        return GrB_OUT_OF_MEMORY;
    }

    while (fgets(line, MAX_LINE, file)) {
        if (fscanf(file, "%lu,%lu", &from, &to) != 2) {
            PRINT_ERROR("Error: couldn't read data\n");
            free_memory(&I, &J);
            fclose(file);
            return GrB_INVALID_VALUE;
        }
        I[count] = from - 1;
        J[count] = to - 1;
        count++;
    }
    if (A != NULL) {
        GrB_Matrix_free(A);
    }
    status = GrB_Matrix_new(A, GrB_FP64, num_nodes, num_nodes);
    if (status != GrB_SUCCESS) {
        PRINT_ERROR("Error: failed to create a matrix\n");
        free_memory(&I, &J);
        fclose(file);
        return status;
    }
    status = GxB_Matrix_build_Scalar(*A, I, J, val, num_edges);
    if (status != GrB_SUCCESS) {
        PRINT_ERROR("Error: failed to create a matrix\n");
        GrB_Matrix_free(A);
        free_memory(&I, &J);
        fclose(file);
        return status;
    }
    GrB_Scalar_free(&val);
    free_memory(&I, &J);
    fclose(file);
    return status;
}
