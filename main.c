#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

int main(int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 7);

    // optimizer settings
    optimset o = get_settings(argv);

    const int n = argc - 6;
    point *start = get_point(n);
    // set initial point from command arguments
    for (int i = 0; i < n; i++) {
        start->x[i] = strtod(argv[i + 6], NULL);
    }
    point *solution = get_point(n);

    // model parameters
    model *m = model_init();

    // get a simplex
    simplex *s = get_simplex(n, o.size, start, m);
    sort(s);

    // begin optimization
    nelder_mead(s, solution, m, &o);

    // print starting point
    fprintf(stderr, "%s     Initial ", GRY);
    cost(n, start, m);
    print_point(n, start, o.places, o.fmt);
    // print solution
    fprintf(stderr, "    %sSolution ", GRY);
    print_point(n, solution, o.places, o.fmt);
    // print stats
    fprintf(stderr, "%s  Iterations/Evaluations%s %d/%d\n", GRY, NRM, s->iterations, s->evaluations);

    return 0;
}
