#include "bench_pagerank.h"
#include <GraphBLAS.h>
#include <LAGraph.h>

typedef struct {
    const char *graph;
    int num_iterations;
} benchmark_config_t;

int main()
{
    GrB_init(GrB_NONBLOCKING);
    LAGraph_Init(NULL);

    benchmark_config_t configs[] = {{"web-Stanford", 200},
                                    {"web-Google", 100},
                                    {"hollywood-2009", 30},
                                    {"indochina-2004", 15},
                                    {NULL, 0}};

    for (int i = 0; configs[i].graph; i++) {
        if (run_experiment(configs[i].graph, configs[i].num_iterations) != 0) {
            fprintf(stderr, "Benchmark failed for %s\n", configs[i].graph);
        }
    }

    GrB_finalize();
    LAGraph_Finalize(NULL);
    return 0;
}