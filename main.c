#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simplex.h"

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 9);

    // optimizer settings
    optimset o = get_settings(argv);

    // model parameters
    model *m = model_init();

    point *centre = get_point(o.n);
    if (o.init_mode == POINT) {
        CHECK(argc == 9 + o.n);
        for (int j = 0; j < o.n; j++) {
            centre->x[j] = strtold(argv[9 + j], NULL);
        }
    } else {
        CHECK(argc == 11);
        o.lower = strtold(argv[9], NULL);
        o.upper = strtold(argv[10], NULL);  CHECK(o.upper > o.lower);
        set_random_coordinates(centre, o.n, o.lower, o.upper);
    }

    simplex *s = nm_simplex(o.n, o.size, centre, o.adaptive);
    fprintf(stderr, o.fmt ? "      %sDiameter%s% .*Le\n" : "      %sDiameter%s% .*Lf\n",
            GRY, NRM, o.places, distance(o.n, s->p, s->p + o.n));

    point *boat = get_point(o.n);
    boat->f = INFINITY;
    int runs = 0, iterations = 0, evaluations = 0;
    do {
        runs++;
        for (int i = 0; i < o.n + 1; i++) {
            cost(o.n, s->p + i, m);
            s->evaluations++;
        }
        sort(s);
        nelder_mead(s, m, &o);
        iterations += s->iterations;
        evaluations += s->evaluations;

        if (s->p[0].f < boat->f) {
            copy_point(o.n, s->p, boat);
            fprintf(stderr, "%5d %8d %8d ", runs, iterations, evaluations);
            print_result(o.n, boat, o.places, o.fmt);
        }

        if (o.init_mode == BULK) {
            set_random_coordinates(centre, o.n, o.lower, o.upper);
            regular_simplex(s, o.size, centre);
        }
    } while (o.init_mode == BULK && evaluations < o.max_evaluations);

    return 0;
}
