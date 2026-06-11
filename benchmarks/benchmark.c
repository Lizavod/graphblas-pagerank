#include "bench_pagerank.h"
#include <GraphBLAS.h>
#include <LAGraph.h>

typedef struct {
    const char *graph, int num_iterations
} benchmark_config_t;

int main()
{
    GrB_init(GrB_NONBLOCKING);
    LAGraph_Init(NULL);

    benchmark_config_t configs[] = {{"web-Stanford", 200},
                                    {"web-Google", 100},
                                    {"hollywood-2009", 50},
                                    {"indochina-2004", 20},
                                    {NULL, 0}};

    for (int i = 0; configs[i].name; i++) {
        run_experiment(configs[i].name, configs[i].num_iterations);
    }

    GrB_finalize();
    return 0;
}