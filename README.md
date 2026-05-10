# GraphBLAS PageRank

[![CI](https://github.com/Lizavod/graphblas-pagerank/actions/workflows/ci.yml/badge.svg)](https://github.com/Lizavod/graphblas-pagerank/actions/workflows/ci.yml)
[![License: Apache 2.0](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)

Реализация алгоритма PageRank на C с использованием библиотеки GraphBLAS (SuiteSparse:GraphBLAS).


## Описание

Алгоритм вычисляет важность вершин в графе на основе структуры связей. Реализация использует линейно-алгебраический подход: граф представляется как разреженная матрица смежности, а итерации PageRank выполняются через умножение матрицы на вектор.


## Быстрый старт

### Требования
*   CMake 3.12+
*   GCC 9+ / Clang 10+
*   SuiteSparse:GraphBLAS 2.x

### Сборка и установка

```bash
git clone https://github.com/Lizavod/graphblas-pagerank.git
cd graphblas-pagerank
mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

sudo make install # Установка в систему
```
### Как использовать

```С 
#include <pagerank/pagerank.h>
```

```bash
gcc main.c -I/usr/local/include -L/usr/local/lib -lpagerank_lib -lgraphblas -lm
```


## API

### Вычисление PageRank графа
`GrB_Info pagerank(GrB_Vector *centrality, int *iters, const GrB_Matrix A, double damping, double tol, int max_iters)`


## Лицензия

Этот проект распространяется под лицензией Apache License 2.0.
См. файл [LICENSE](LICENSE) для деталей.
