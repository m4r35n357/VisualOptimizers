#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simplex.h"

int main(int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 10);

    // optimizer settings
    optimset o = get_settings(argv, false);

    // model parameters
    model *m = model_init();

    // set initial point from command arguments
    srand((unsigned int)time(NULL));
    point *best = get_random_point(o.n, o.lower, o.upper);
    cost(o.n, best, m);
    point *trial = get_point(o.n);
    for (int i = 0; i < o.random_init * o.n; i++) {
        get_random_coordinates(trial, o.n, o.lower, o.upper);
        cost(o.n, trial, m);
        if (trial->f < best->f) copy_point(o.n, trial, best);
    }

    // default simplex . . .
    simplex *s1 = mds_simplex(o.n, o.size, best);
    for (int i = 0; i < s1->n + 1; i++) {  // initial cost at simplex vertices
        cost(s1->n, s1->p + i, m);
        s1->evaluations++;
    }
    sort(s1);

    // . . . and its "dual"
    simplex *s2 = mds_simplex(o.n, o.size, best);
    for (int i = 0; i < s2->n + 1; i++) {  // form "dual" by projecting vertices through the centre
        project(s2->p + i, s2, m, 1.0L, s2->p + i, best);
    }
    sort(s2);

    // print starting point
    fprintf(stderr, "%s       Initial  ", GRY);
    print_result(o.n, best, o.places, o.fmt);
    fprintf(stderr, o.fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter%s    % .*Lf\n",
            GRY, NRM, o.places, distance(s1->n, s1->p, s1->p + s1->n));

    // begin optimization
    multidirectional_search(s1, m, &o);
    multidirectional_search(s2, m, &o);
    best = s1->p[0].f <= s2->p[0].f ? s1->p : s2->p;

    // print solution 1
    fprintf(stderr, "%s%s1%s ", s1->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s1->iterations, s1->evaluations);
    print_result(s1->n, s1->p, o.places, o.fmt);

    // print solution 2
    fprintf(stderr, "%s%s2%s ", s2->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s2->iterations, s2->evaluations);
    print_result(s2->n, s2->p, o.places, o.fmt);

    return 0;
}
