// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#include "mtx_reader.h"
#include <GraphBLAS.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 1024

typedef struct {
    char object[32];
    char format[32];
    char datatype[32];
    char symmetry[32];
    GrB_Index nedges;
    GrB_Index nrows;
    GrB_Index ncols;
} mm_file_info_t;

static int is_valid_header(mm_file_info_t *info, const char *banner)
{
    if (strcmp(banner, "%%MatrixMarket") != 0) {
        PRINT_ERROR("Error: the header is invalid\n");
        return 1;
    }
    if (strcmp(info->object, "matrix") != 0 &&
        strcmp(info->object, "vector") != 0) {
        PRINT_ERROR("Error: invalid object in the header\n");
        return 1;
    }
    if (strcmp(info->format, "coordinate") != 0 &&
        strcmp(info->format, "array") != 0) {
        PRINT_ERROR("Error: invalid format in the header\n");
        return 1;
    }
    if (strcmp(info->datatype, "real") != 0 &&
        strcmp(info->datatype, "double") != 0 &&
        strcmp(info->datatype, "pattern") != 0 &&
        strcmp(info->datatype, "integer") != 0 &&
        strcmp(info->datatype, "complex") != 0) {
        PRINT_ERROR("Error: invalid datatype in the header\n");
        return 1;
    }
    if (strcmp(info->symmetry, "general") != 0 &&
        strcmp(info->symmetry, "symmetric") != 0 &&
        strcmp(info->symmetry, "skew-symmetric") != 0 &&
        strcmp(info->symmetry, "hermitian") != 0) {
        PRINT_ERROR("Error: invalid symmetry in the header\n");
        return 1;
    }
    return 0;
}

static int is_valid_for_pagerank(mm_file_info_t *info)
{
    if (strcmp(info->object, "matrix") != 0) {
        PRINT_ERROR("Error: the file must contain the matrix\n");
        return 1;
    }
    if (strcmp(info->format, "coordinate") != 0) {
        PRINT_ERROR("Error: the 'array' dense format is not supported\n");
        return 1;
    }
    if (strcmp(info->datatype, "complex") == 0) {
        PRINT_ERROR("Error: complex-type is not supported for PageRank\n");
        return 1;
    }
    if (strcmp(info->symmetry, "skew-symmetric") == 0 ||
        strcmp(info->symmetry, "hermitian") == 0) {
        PRINT_ERROR("Error: The '%s' symmetry is not supported\n",
                    info->symmetry);
        return 1;
    }
    if (info->nrows != info->ncols) {
        PRINT_ERROR("Error: the matrix must be square %lu != %lu\n",
                    info->nrows, info->ncols);
        return 1;
    }
    return 0;
}

static int skip_comments_and_spaces(const char **ptr)
{
    while (**ptr && isspace((unsigned char)**ptr)) {
        (*ptr)++;
    }
    if (**ptr == '\0' || **ptr == '\n' || **ptr == '%') {
        return 1;
    }
    return 0;
}

static int read_u64(const char **ptr, uint64_t *value)
{
    char *end = NULL;
    errno = 0;
    unsigned long long val = 0;
    if (skip_comments_and_spaces(ptr) == 1) {
        return 1;
    }
    val = strtoull(*ptr, &end, 10);
    if (end == *ptr || errno == ERANGE) {
        return 1;
    }
    *value = (uint64_t)val;
    *ptr = end;
    return 0;
}

static int read_f64(const char **ptr, double *value)
{
    char *end = NULL;
    errno = 0;
    if (skip_comments_and_spaces(ptr) == 1) {
        return 1;
    }
    *value = strtod(*ptr, &end);
    if (end == *ptr || errno == ERANGE) {
        return 1;
    }
    *ptr = end;
    return 0;
}

static int read_header(FILE *file, mm_file_info_t *info)
{
    char line[MAX_LINE];
    char banner[32] = {0};
    char *ptr = NULL;
    int count = 0;
    while (fgets(line, MAX_LINE, file)) {
        ptr = line;
        if (skip_comments_and_spaces(&ptr) == 1) {
            continue;
        }
        count = sscanf(ptr, "%s %s %s %s %s", banner, info->object,
                       info->format, info->datatype, info->symmetry);
        if (count < 4) {
            PRINT_ERROR("Error: could not read the header\n");
            return 1;
        }
        if (count == 4) {
            strcpy(info->symmetry, "general");
        }
        break;
    }
    if (count == 0) {
        PRINT_ERROR("Error: the header is missing\n");
        return 1;
    }
    if (is_valid_header(info, banner) != 0) {
        return 1;
    }
    return 0;
}

static int read_dimensions(FILE *file, mm_file_info_t *info)
{
    char line[MAX_LINE];
    char *ptr = NULL;
    uint64_t rows = 0, cols = 0, edges = 0;
    while (fgets(line, MAX_LINE, file)) {
        ptr = line;
        if (skip_comments_and_spaces(&ptr)) {
            continue;
        }
        if (read_u64(&ptr, &rows) != 0 || read_u64(&ptr, &cols) != 0 ||
            read_u64(&ptr, &edges) != 0) {
            PRINT_ERROR("Error: couldn't read the dimensions\n");
            return 1;
        }
        break;
    }
    if (rows == 0) {
        PRINT_ERROR("Error: dimensions is missing\n");
        return 1;
    }
    info->nrows = (GrB_Index)rows;
    info->ncols = (GrB_Index)cols;
    info->nedges = (GrB_Index)edges;
    return 0;
}

static void free_memory(GrB_Index **I, GrB_Index **J, double **X)
{
    free(*I);
    free(*J);
    free(*X);
    *I = NULL;
    *J = NULL;
    *X = NULL;
}

static GrB_Info read_data(FILE *file, mm_file_info_t *info, GrB_Matrix **A)
{
    GrB_Info status = GrB_SUCCESS;
    char line[MAX_LINE];
    char *ptr = NULL;
    uint64_t row = -1, col = -1;
    double val = 1;
    GrB_Index *I = malloc(info->nedges * sizeof(GrB_Index));
    GrB_Index *J = malloc(info->nedges * sizeof(GrB_Index));
    double *X = malloc(info->nedges * sizeof(double));
    if (!I || !J || !X) {
        free_memory(&I, &J, &X, A);
        PRINT_ERROR("Error: couldn't allocate memory\n");
        return GrB_OUT_OF_MEMORY;
    }
    uint64_t count = 0;
    while (fgets(line, MAX_LINE, file)) {
        ptr = line;
        if (skip_comments_and_spaces(&ptr)) {
            continue;
        }
        if (read_u64(&ptr, &row) != 0 || read_u64(&ptr, &col) != 0) {
            PRINT_ERROR("Error: couldn't read the data\n");
            free_memory(&I, &J, &X);
            return GrB_INVALID_VALUE;
        }
        if (strcmp(info->datatype, "pattern") != 0) {
            if (read_f64(&ptr, &val) != 0) {
                PRINT_ERROR("Error: couldn't read the data\n");
                free_memory(&I, &J, &X);
                return GrB_INVALID_VALUE;
            }
        }
        if (row >= info->nrows || col >= info->ncols) { // ТИПЫ!!!!
            PRINT_ERROR(
                "Warning: edge indices are outside the allowed range\n");
            continue;
        }
        I[count] = (GrB_Index)(row - 1);
        J[count] = (GrB_Index)(col - 1);
        X[count] = val;
        count++;
        if (count == info->nedges) {
            break;
        }
    }
    if (count < info->nedges) {
        PRINT_ERROR("Warning: expected %lu edges, read %lu edges\n",
                    info->nedges, count);
    }
    if (*A != NULL) {
        GrB_Matrix_free(*A);
    }
    status = GrB_Matrix_new(A, GrB_FP64, info->nrows, info->ncols);
    if (status != GrB_SUCCESS) {
        free_memory(&I, &J, &X);
        return status;
    }
    status = GrB_Matrix_build_FP64(A, I, J, X, count, GrB_SECOND_FP64);
    if (status != GrB_SUCCESS) {
        PRINT_ERROR("Error: failed to create a matrix\n");
        GrB_Matrix_free(*A);
        free_memory(&I, &J, &X);
        return status;
    }
    free_memory(&I, &J, &X);
    return status;
}

GrB_Info read_matrix(const char *filename, GrB_Matrix *A)
{
    FILE *file = fopen(filename, "r");
    GrB_Info status = GrB_SUCCESS;
    mm_file_info_t info;
    if (!file) {
        PRINT_ERROR("Error: could not open the file %s\n", filename);
        return GrB_PANIC;
    }
    if (read_header(file, &info)) {
        fclose(file);
        return GrB_INVALID_VALUE;
    }
    if (is_valid_for_pagerank(&info) != 0) {
        fclose(file);
        return GrB_INVALID_VALUE;
    }
    if (read_dimensions(file, &info)) {
        fclose(file);
        return GrB_INVALID_VALUE;
    }
    if ((status = read_data(file, &info, &A)) != GrB_SUCCESS) {
        fclose(file);
        return status;
    }
    return status;
}