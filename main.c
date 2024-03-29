#include <stdio.h>
#include <stdlib.h>
#include "simplex.h"

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 9);

    // optimizer settings
    optimset o = get_settings(argv, false);

    // model parameters
    model *m = model_init();

    point *start = get_point(o.n);
    if (o.init_mode) {  // random initial point in range
        CHECK(argc == 11);
        o.lower = strtold(argv[9], NULL);
        o.upper = strtold(argv[10], NULL);  CHECK(o.upper > o.lower);
        set_random_coordinates(start, o.n, o.lower, o.upper);
    } else {  // set initial point from command arguments
        CHECK(argc == 9 + o.n);
        for (int j = 0; j < o.n; j++) {
            start->x[j] = strtold(argv[9 + j], NULL);
        }
    }
    cost(o.n, start, m);

    simplex *s1 = nm_simplex(o.n, o.size, start, o.adaptive);
    simplex *s2 = nm_simplex(o.n, o.size, start, o.adaptive);
    fprintf(stderr, o.fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter%s    % .*Lf\n",
            GRY, NRM, o.places, distance(o.n, s1->p, s1->p + o.n));

    int runs = 1, iterations = 0, evaluations = 1;
    point *boat = get_point(o.n);
    copy_point(o.n, start, boat);
    do {
        for (int i = 0; i < o.n + 1; i++) {
            cost(o.n, s1->p + i, m);
            s1->evaluations++;
            project(s2->p + i, s2, m, 1.0L, s2->p + i, start);
        }
        sort(s1);
        sort(s2);
        nelder_mead(s1, m, &o);
        nelder_mead(s2, m, &o);
        iterations += s1->iterations + s2->iterations;
        evaluations += s1->evaluations + s2->evaluations;

        fprintf(stderr, "\r                         \r%5d %8d %8d ", runs, iterations, evaluations);
        point *best = s1->p[0].f <= s2->p[0].f ? s1->p : s2->p;
        if (best->f < boat->f) {
            copy_point(o.n, best, boat);
            fprintf(stderr, "\r%5d %8d %8d ", runs, iterations, evaluations);
            print_result(o.n, boat, o.places, o.fmt);
        }

        if (o.init_mode) {
            set_random_coordinates(start, o.n, o.lower, o.upper);
            regular_simplex(s1, o.size, start);
            regular_simplex(s2, o.size, start);
        }
    } while (runs++ < o.init_mode);

    if (o.init_mode > 1) {
        fprintf(stderr, "\r%5d %8d %8d\n", runs - 1, iterations, evaluations);
    }

    return 0;
}
