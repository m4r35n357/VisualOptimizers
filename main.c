
#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main(int argc, const char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc > 2);

    // optimizer settings
    optimset opt = {
        .tolx = 1.0e-9L,
        .tolf = 1.0e-9L,
        .max_iter = 5000,
        .max_eval = 5000,
        .verbose = (int)strtol(argv[1], NULL, BASE)
    }; CHECK(opt.verbose == 0 || opt.verbose == 1);

    point start, solution;
    const int n = argc - 2;
    start.x = malloc((size_t)n * sizeof(real));    CHECK(start.x);
    solution.x = malloc((size_t)n * sizeof(real)); CHECK(solution.x);

    // read initial point from command arguments
    for (int i = 0; i < n; i++) {
        start.x[i] = strtod(argv[i + 2], NULL);
    }

    // cost function parameters
    model *m = get_parameters();

    // begin optimization
    nelder_mead(n, &start, &solution, m, &opt);

    // evaluate and print starting point
    printf("%s     Initial ", GRY);
    cost(n, &start, m);
    print_point(n, &start);
    // print solution
    printf("    %sSolution ", GRY);
    print_point(n, &solution);

    // free memory
    free(start.x);
    free(solution.x);

    return 0;
}
