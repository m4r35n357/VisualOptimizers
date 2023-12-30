
#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

int main(int argc, const char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 12);

    // optimizer settings
    optimset opt = {
        .precision = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .debug = (int)strtol(argv[3], NULL, BASE),
        .x_tolerance = strtold(argv[4], NULL),
        .f_tolerance = strtold(argv[5], NULL),
        .max_iterations = (int)strtol(argv[6], NULL, BASE),
        .max_evaluations = (int)strtol(argv[7], NULL, BASE),
        .adaptive = (int)strtol(argv[8], NULL, BASE),
        .size = strtold(argv[9], NULL)
    };
    CHECK(opt.precision >= 3 && opt.precision <= 36);
    CHECK(opt.debug == 0 || opt.debug == 1);
    CHECK(opt.x_tolerance >= 1.0e-36L && opt.x_tolerance <= 1.0e-3L);
    CHECK(opt.f_tolerance >= 1.0e-36L && opt.f_tolerance <= 1.0e-3L);
    CHECK(opt.max_iterations >= 1 && opt.max_iterations <= 100000);
    CHECK(opt.max_evaluations >= 1 && opt.max_evaluations <= 100000);
    CHECK(opt.adaptive == 0 || opt.adaptive == 1);
    CHECK(opt.size >= 1.0e-12L && opt.size <= 1.0e3L);

    const int n = argc - 10;
    point *start = get_point(n);
    // set initial point from command arguments
    for (int i = 0; i < n; i++) {
        start->x[i] = strtod(argv[i + 10], NULL);
    }
    point *solution = get_point(n);

    // parameters & cost function
    model *m = get_model();

    // get a simplex
    simplex *s = get_simplex(n, opt.size, start);

    // begin optimization
    nelder_mead(s, solution, m, &opt);

    // print starting point
    printf("%s     Initial ", GRY);
    m->cost(start, n, m->p);
    print_point(n, start, opt.precision, opt.fmt);
    // print solution
    printf("    %sSolution ", GRY);
    print_point(n, solution, opt.precision, opt.fmt);
    // print stats
    printf("%s  Iterations/Evaluations%s %d/%d\n", GRY, NRM, s->iterations, s->evaluations);

    return 0;
}
