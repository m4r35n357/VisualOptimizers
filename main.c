
#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

int main(int argc, const char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 12);

    // optimizer settings
    optimset opt = {
        .diplay_precision = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .verbose = (int)strtol(argv[3], NULL, BASE),
        .tolx = strtold(argv[4], NULL),
        .tolf = strtold(argv[5], NULL),
        .max_iter = (int)strtol(argv[6], NULL, BASE),
        .max_eval = (int)strtol(argv[7], NULL, BASE),
        .adaptive_scaling = (int)strtol(argv[8], NULL, BASE),
        .simplex_scaling = strtold(argv[9], NULL)
    };
    CHECK(opt.diplay_precision >= 3 && opt.diplay_precision <= 36);
    CHECK(opt.verbose == 0 || opt.verbose == 1);
    CHECK(opt.tolx >= 1.0e-36L && opt.tolx <= 1.0e-3L);
    CHECK(opt.tolf >= 1.0e-36L && opt.tolf <= 1.0e-3L);
    CHECK(opt.max_iter >= 1 && opt.max_iter <= 100000);
    CHECK(opt.max_eval >= 1 && opt.max_eval <= 100000);
    CHECK(opt.adaptive_scaling == 0 || opt.adaptive_scaling == 1);
    CHECK(opt.simplex_scaling >= 1.0e-12L && opt.simplex_scaling <= 1.0e3L);

    const int n = argc - 10;
    point start, solution;
    start.x = malloc((size_t)n * sizeof(real));    CHECK(start.x);
    // set initial point from command arguments
    for (int i = 0; i < n; i++) {
        start.x[i] = strtod(argv[i + 10], NULL);
    }
    solution.x = malloc((size_t)n * sizeof(real)); CHECK(solution.x);

    // cost function parameters
    model *m = get_model();

    // get a simplex
    simplex *s = get_simplex(n, opt.simplex_scaling, &start, m);

    // begin optimization
    nelder_mead(s, &solution, m, &opt);

    // print starting point
    printf("%s     Initial ", GRY);
    m->c(n, &start, m->p);
    print_point(n, &start, opt.diplay_precision, opt.fmt);
    // print solution
    printf("    %sSolution ", GRY);
    print_point(n, &solution, opt.diplay_precision, opt.fmt);
    // print stats
    printf("%s  Iterations/Evaluations%s %d/%d\n", GRY, NRM, s->iterations, s->evaluations);

    return 0;
}
