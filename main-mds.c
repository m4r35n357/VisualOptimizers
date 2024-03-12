#include <stdio.h>
#include <stdlib.h>
#include "simplex.h"

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 8);

    // optimizer settings
    optimset o = get_settings(argv, false);

    // model parameters
    model *m = model_init();

    point *start = get_point(o.n);
    if (o.init_mode) {  // random initial point in range
        CHECK(argc == 10);
        o.lower = strtold(argv[8], NULL);
        o.upper = strtold(argv[9], NULL);  CHECK(o.upper > o.lower);
        set_random_coordinates(start, o.n, o.lower, o.upper);
    } else {  // set initial point from command arguments
        CHECK(argc == 8 + o.n);
        for (int j = 0; j < o.n; j++) {
            start->x[j] = strtold(argv[8 + j], NULL);
        }
    }
    cost(o.n, start, m);

    // default simplex . . .
    simplex *s1 = mds_simplex(o.n, o.size, start);
    for (int i = 0; i < s1->n + 1; i++) {  // initial cost at simplex vertices
        cost(s1->n, s1->p + i, m);
        s1->evaluations++;
    }
    sort(s1);

    // . . . and its "dual"
    simplex *s2 = mds_simplex(o.n, o.size, start);
    for (int i = 0; i < s2->n + 1; i++) {  // form "dual" by projecting vertices through the centre
        project(s2->p + i, s2, m, 1.0L, s2->p + i, start);
    }
    sort(s2);

    // print starting point
    fprintf(stderr, "%s       Initial  ", GRY);
    print_result(o.n, start, o.places, o.fmt);
    fprintf(stderr, o.fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter%s    % .*Lf\n",
            GRY, NRM, o.places, distance(s1->n, s1->p, s1->p + s1->n));

    // begin optimization
    multidirectional_search(s1, m, &o);
    multidirectional_search(s2, m, &o);
    point *best = s1->p[0].f <= s2->p[0].f ? s1->p : s2->p;

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
