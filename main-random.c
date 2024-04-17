#include <stdio.h>
#include <stdlib.h>
#include "simplex.h"

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // optimizer settings
    randomize();
    optimset o = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .init_mode = (int)strtol(argv[4], NULL, BASE),
        .adaptive = (int)strtol(argv[5], NULL, BASE),  // used as progress flag
        .step_mode = false
    };
    CHECK(o.places >= 1 && o.places <= 36);
    CHECK(o.fmt == 0 || o.fmt == 1);
    CHECK(o.n >= 1);
    CHECK(o.init_mode >= 0 && o.init_mode <= 100000000);

    // model parameters
    model *m = model_init();

    point *start = get_point(o.n);
    o.lower = strtold(argv[6], NULL);
    o.upper = strtold(argv[7], NULL);  CHECK(o.upper > o.lower);
    set_random_coordinates(start, o.n, o.lower, o.upper);
    cost(o.n, start, m);

    int runs = 1, evaluations = 1;
    point *boat = get_point(o.n);
    copy_point(o.n, start, boat);
    do {
        cost(o.n, start, m);
        evaluations++;

        if (o.adaptive) {
            fprintf(stderr, "\r                         \r%8d %8d  ", runs, evaluations);
            if (start->f < boat->f) {
                copy_point(o.n, start, boat);
                fprintf(stderr, "\r%8d %8d  ", runs, evaluations);
                print_result(o.n, boat, o.places, o.fmt);
            }
        } else {
        	if (start->f < boat->f) copy_point(o.n, start, boat);
        }

        set_random_coordinates(start, o.n, o.lower, o.upper);
    } while (runs++ < o.init_mode);

    if (! o.adaptive) {
        fprintf(stderr, "\r%8d %8d  ", runs - 1, evaluations);
        print_result(o.n, boat, o.places, o.fmt);
    } else {
        fprintf(stderr, "\r%8d %8d\n", runs - 1, evaluations);
    }

    return 0;
}
