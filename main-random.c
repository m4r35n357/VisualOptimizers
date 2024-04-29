#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simplex.h"

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 7);

    // optimizer settings
    randomize();
    optimset o = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .max_evaluations = (int)strtol(argv[4], NULL, BASE),
        .step_mode = false
    };
    CHECK(o.places >= 1 && o.places <= 36);
    CHECK(o.fmt == 0 || o.fmt == 1);
    CHECK(o.n >= 1);
    CHECK(o.init_mode >= 0 && o.init_mode <= 100000000);

    // model parameters
    model *m = model_init();

    point *centre = get_point(o.n);
    o.lower = strtold(argv[5], NULL);
    o.upper = strtold(argv[6], NULL);  CHECK(o.upper > o.lower);
    set_random_coordinates(centre, o.n, o.lower, o.upper);
    cost(o.n, centre, m);

    point *boat = get_point(o.n);
    boat->f = INFINITY;
    int runs = 0, evaluations = 1;
    do {
        runs++;
        cost(o.n, centre, m);
        evaluations++;

        if (centre->f < boat->f) {
            copy_point(o.n, centre, boat);
        }

        set_random_coordinates(centre, o.n, o.lower, o.upper);
    } while (evaluations < o.max_evaluations);

    fprintf(stderr, "%8d %8d  ", runs, evaluations);
    print_result(o.n, boat, o.places, o.fmt);

    return 0;
}
