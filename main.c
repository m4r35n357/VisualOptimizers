
#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

int main(int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 9);

    // optimizer settings
    optimset o = get_settings(argv);

    const int n = argc - 7;
    point *start = get_point(n);
    // set initial point from command arguments
    for (int i = 0; i < n; i++) {
        start->x[i] = strtod(argv[i + 7], NULL);
    }
    point *solution = get_point(n);

    // model parameters
    model *m = get_parameters();

    // get a simplex
    simplex *s = get_simplex(n, o.size, start, m);

    // begin optimization
    nelder_mead(s, solution, m, &o);

    // print starting point
    printf("%s     Initial ", GRY);
    cost(n, start, m);
    print_point(n, start, o.places, o.fmt);
    // print solution
    printf("    %sSolution ", GRY);
    print_point(n, solution, o.places, o.fmt);
    // print stats
    printf("%s  Iterations/Evaluations%s %d/%d\n", GRY, NRM, s->iterations, s->evaluations);

    return 0;
}
