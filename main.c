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

    simplex *s = nm_simplex(o.n, o.size, start, o.adaptive);
    fprintf(stderr, o.fmt ? "      %sDiameter%s% .*Le\n" : "      %sDiameter%s% .*Lf\n",
            GRY, NRM, o.places, distance(o.n, s->p, s->p + o.n));

    int runs = 1, iterations = 0, evaluations = 1;
    point *boat = get_point(o.n);
    copy_point(o.n, start, boat);
    do {
        for (int i = 0; i < o.n + 1; i++) {
            cost(o.n, s->p + i, m);
            s->evaluations++;
        }
        sort(s);
        nelder_mead(s, m, &o);
        iterations += s->iterations;
        evaluations += s->evaluations;

        fprintf(stderr, "\r                         \r%5d %8d %8d ", runs, iterations, evaluations);
        if (s->p[0].f < boat->f) {
            copy_point(o.n, s->p, boat);
            fprintf(stderr, "\r%5d %8d %8d ", runs, iterations, evaluations);
            print_result(o.n, boat, o.places, o.fmt);
        }

        if (o.init_mode) {
            set_random_coordinates(start, o.n, o.lower, o.upper);
            regular_simplex(s, o.size, start);
        }
    } while (runs++ < o.init_mode);

    if (o.init_mode > 1) {
        fprintf(stderr, "\r%5d %8d %8d\n", runs - 1, iterations, evaluations);
    }

    return 0;
}
