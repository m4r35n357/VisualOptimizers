#include <stdio.h>
#include <stdlib.h>
#include "simplex.h"

int main(int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 8);
    const int n = argc - 7;

    // optimizer settings
    optimset o = get_settings(argv, false);

    // model parameters
    model *m = model_init();

    // set initial point from command arguments
    point *start = get_point(n);
    for (int j = 0; j < n; j++) {
        start->x[j] = strtod(argv[j + 7], NULL);
    }

    // default simplex . . .
    simplex *s1 = get_nm_simplex(n, o.size, start);
    for (int i = 0; i < s1->n + 1; i++) {  // initial cost at simplex vertices
        cost(s1->n, s1->p + i, m);
        s1->evaluations++;
    }
    sort(s1);

    // begin optimization
    nelder_mead(s1, m, &o);

    // . . . and its "dual"
    simplex *s2 = get_nm_simplex(n, o.size, start);
    for (int i = 0; i < s2->n + 1; i++) {  // form "dual" by projecting vertices through the centre
        project(s2->p + i, s2, m, 1.0L, s2->p + i, start);
    }
    sort(s2);

    // begin optimization
    nelder_mead(s2, m, &o);

    point *best = s1->p[0].f <= s2->p[0].f ? s1->p : s2->p;
    // print solution 1
    fprintf(stderr, "%s%s1%s ", s1->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s1->iterations, s1->evaluations);
    print_point(s1->n, s1->p, o.places, o.fmt);
    // print solution 2
    fprintf(stderr, "%s%s2%s ", s2->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s2->iterations, s2->evaluations);
    print_point(s2->n, s2->p, o.places, o.fmt);

    return 0;
}
