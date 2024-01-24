#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

int main(int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 7);

    // optimizer settings
    optimset o = get_settings(argv, false);

    const int n = argc - 6;
    point *start = get_point(n);
    // set initial point from command arguments
    for (int i = 0; i < n; i++) {
        start->x[i] = strtod(argv[i + 6], NULL);
    }
    point *solution1 = get_point(n);
    point *solution2 = get_point(n);

    // model parameters
    model *m = model_init();

    // default simplex . . .
    simplex *s1 = get_simplex(n, o.size, start);
    for (int i = 0; i < s1->n + 1; i++) {  // initial cost at simplex vertices
        cost(s1->n, s1->p + i, m);
        s1->evaluations++;
    }
    sort(s1);

    // print starting point
    fprintf(stderr, "%s       Initial  ", GRY);
    cost(n, start, m);
    print_point(n, start, o.places, o.fmt);
    fprintf(stderr, o.fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter%s    % .*Lf\n",
            GRY, NRM, o.places, distance(s1->n, s1->p, s1->p + s1->n));

    // begin optimization
    nelder_mead(s1, solution1, m, &o);
    // print solution
    fprintf(stderr, "  %s1%s ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s1->iterations, s1->evaluations);
    print_point(n, solution1, o.places, o.fmt);

    // . . . and its "dual"
    simplex *s2 = get_simplex(n, o.size, start);
    for (int i = 0; i < s2->n + 1; i++) {  // form "dual" by projecting vertices through the centre
        project(s2->p + i, s2, m, start, 1.0L, s2->p + i, start);
    }
    sort(s2);

    // begin optimization
    nelder_mead(s2, solution2, m, &o);
    // print solution
    fprintf(stderr, "  %s2%s ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s2->iterations, s2->evaluations);
    print_point(n, solution2, o.places, o.fmt);

    return 0;
}
