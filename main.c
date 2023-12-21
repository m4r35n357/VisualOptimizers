
#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main(int argc, const char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc > 6);

    // optimizer settings
    optimset opt = {
        .tolx = 1.0e-9L,
        .tolf = 1.0e-9L,
        .max_iter = 5000,
        .max_eval = 5000,
        .diplay_precision = (int)strtol(argv[1], NULL, BASE),
        .verbose = (int)strtol(argv[2], NULL, BASE),
        .adaptive_scaling = (int)strtol(argv[3], NULL, BASE),
        .simplex_scaling = strtold(argv[4], NULL)
    };
    CHECK(opt.diplay_precision >= 3 && opt.diplay_precision <= 36);
    CHECK(opt.verbose == 0 || opt.verbose == 1);
    CHECK(opt.adaptive_scaling == 0 || opt.adaptive_scaling == 1);
    CHECK(opt.simplex_scaling >= 1.0e-3L && opt.simplex_scaling <= 1.0e3L);

    point start, solution;
    const int n = argc - 5;
    start.x = malloc((size_t)n * sizeof(real));    CHECK(start.x);
    solution.x = malloc((size_t)n * sizeof(real)); CHECK(solution.x);

    // read initial point from command arguments
    for (int i = 0; i < n; i++) {
        start.x[i] = strtod(argv[i + 5], NULL);
    }

    // cost function parameters
    model *m = get_parameters();

    // begin optimization
    nelder_mead(n, &start, &solution, m, &opt);

    // evaluate and print starting point
    printf("%s     Initial ", GRY);
    cost(n, &start, m);
    print_point(n, &start, opt.diplay_precision);
    // print solution
    printf("    %sSolution ", GRY);
    print_point(n, &solution, opt.diplay_precision);

    // free memory
    free(start.x);
    free(solution.x);

    return 0;
}
