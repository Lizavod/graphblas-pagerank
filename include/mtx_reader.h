// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

#ifndef MTX_READER_H
#define MTX_READER_H

#include <GraphBLAS.h>

GrB_Info read_matrix(const char *filename, GrB_Matrix *A);

#endif