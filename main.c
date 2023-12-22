
#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

int main(int argc, const char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 11);

    // optimizer settings
    optimset opt = {
        .diplay_precision = (int)strtol(argv[1], NULL, BASE),
        .verbose = (int)strtol(argv[2], NULL, BASE),
        .tolx = strtold(argv[3], NULL),
        .tolf = strtold(argv[4], NULL),
        .max_iter = (int)strtol(argv[5], NULL, BASE),
        .max_eval = (int)strtol(argv[6], NULL, BASE),
        .adaptive_scaling = (int)strtol(argv[7], NULL, BASE),
        .simplex_scaling = strtold(argv[8], NULL)
    };
    CHECK(opt.diplay_precision >= 3 && opt.diplay_precision <= 36);
    CHECK(opt.verbose == 0 || opt.verbose == 1);
    CHECK(opt.tolx >= 1.0e-36L && opt.tolx <= 1.0e-3L);
    CHECK(opt.tolf >= 1.0e-36L && opt.tolf <= 1.0e-3L);
    CHECK(opt.max_iter >= 1 && opt.max_iter <= 100000);
    CHECK(opt.max_eval >= 1 && opt.max_eval <= 100000);
    CHECK(opt.adaptive_scaling == 0 || opt.adaptive_scaling == 1);
    CHECK(opt.simplex_scaling >= 1.0e-3L && opt.simplex_scaling <= 1.0e3L);

    point start, solution;
    const int n = argc - 9;
    start.x = malloc((size_t)n * sizeof(real));    CHECK(start.x);
    solution.x = malloc((size_t)n * sizeof(real)); CHECK(solution.x);

    // read initial point from command arguments
    for (int i = 0; i < n; i++) {
        start.x[i] = strtod(argv[i + 9], NULL);
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
