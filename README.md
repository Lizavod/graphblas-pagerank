# GraphBLAS PageRank

[![CI](https://github.com/Lizavod/graphblas-pagerank/actions/workflows/ci.yml/badge.svg)](https://github.com/Lizavod/graphblas-pagerank/actions/workflows/ci.yml)
[![License: Apache 2.0](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)

Реализация алгоритма PageRank на C с использованием библиотеки GraphBLAS (SuiteSparse:GraphBLAS).


## Описание

Алгоритм вычисляет важность вершин в графе на основе структуры связей. Реализация использует линейно-алгебраический подход: граф представляется как разреженная матрица смежности, а итерации PageRank выполняются через умножение матрицы на вектор.


## Требования
*   CMake 3.12+
*   GCC 9+ / Clang 10+
*   SuiteSparse:GraphBLAS 7.4.0
  

## Быстрый старт

### Сборка и установка

```bash
git clone https://github.com/Lizavod/graphblas-pagerank.git
cd graphblas-pagerank
mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

sudo make install # Установка в систему
```

### Запуск тестов

```bash
cd build
ctest --output-on-failure
```


## Как использовать

### Пример кода

```С 
#include <pagerank/pagerank.h>
#include <GraphBLAS.h>
#include <stdio.h>

int main()
{
    // Инициализация GraphBLAS (ОДИН раз)
    GrB_init(GrB_NONBLOCKING);

    // Создание матрицы смежности (пример)
    GrB_Matrix A;
    GrB_Matrix_new(&A, GrB_FP64, 4, 4);
    // ... заполнение матрицы A ...

    // Вычисление PageRank
    GrB_Vector result;
    int iters;
    double damping = 0.85;
    double tol = 1e-6;
    int max_iters = 100;

    pagerank(&result, &iters, A, damping, tol, max_iters);

    printf("Сходимость достигнута за %d итераций\n", iters);

    // Освобождение ресурсов
    GrB_Vector_free(&result);
    GrB_Matrix_free(&A);
    GrB_finalize();
    return 0;
}
```

### Компиляция

```bash
gcc main.c -I/usr/local/include -L/usr/local/lib -lpagerank_lib -lgraphblas -lm
```


## API

### Вычисление PageRank графа

```С 
GrB_Info pagerank(
    GrB_Vector *centrality,  // [out] Вектор рангов
    int *iters,              // [out] Число выполненных итераций
    const GrB_Matrix A,      // [in]  Матрица смежности
    double damping,          // [in]  Коэффициент затухания
    double tol,              // [in]  Порог сходимости
    int max_iters            // [in]  Максимальное число итераций
);
```

## Лицензия

Этот проект распространяется под лицензией Apache License 2.0.
См. файл [LICENSE](LICENSE) для деталей.
