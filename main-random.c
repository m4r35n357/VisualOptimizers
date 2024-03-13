#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simplex.h"

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 7);

    // optimizer settings
    srand((unsigned int)time(NULL));
    optimset o = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .init_mode = (int)strtol(argv[4], NULL, BASE),
        .step_mode = false
    };
    CHECK(o.places >= 3 && o.places <= 36);
    CHECK(o.fmt == 0 || o.fmt == 1);
    CHECK(o.n >= 1);
    CHECK(o.init_mode >= 0 && o.init_mode <= 100000000);

    // model parameters
    model *m = model_init();

    point *start = get_point(o.n);
    o.lower = strtold(argv[5], NULL);
    o.upper = strtold(argv[6], NULL);  CHECK(o.upper > o.lower);
    set_random_coordinates(start, o.n, o.lower, o.upper);
    cost(o.n, start, m);

    int runs = 1, evaluations = 1;
    point *boat = get_point(o.n);
    copy_point(o.n, start, boat);
    do {
        cost(o.n, start, m);
        evaluations++;

        if (start->f < boat->f) copy_point(o.n, start, boat);

        set_random_coordinates(start, o.n, o.lower, o.upper);
    } while (runs++ < o.init_mode);

    fprintf(stderr, "\r%7d %6d  ", runs - 1, evaluations);
    print_result(o.n, boat, o.places, o.fmt);

    return 0;
}
