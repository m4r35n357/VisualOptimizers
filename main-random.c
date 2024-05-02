#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simplex.h"

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 7);

    randomize();

    int places = (int)strtol(argv[1], NULL, BASE); CHECK(places >= 1 && places <= 36);
    int fmt = (int)strtol(argv[2], NULL, BASE); CHECK(fmt == 0 || fmt == 1);
    int n = (int)strtol(argv[3], NULL, BASE); CHECK(n >= 1);
    int max_evaluations = (int)strtol(argv[4], NULL, BASE); CHECK(max_evaluations >= 0 && max_evaluations <= 100000000);

    // model parameters
    model *m = model_init();

    point *centre = get_point(n);
    real lower = strtold(argv[5], NULL);
    real upper = strtold(argv[6], NULL);  CHECK(upper > lower);
    set_random_coordinates(centre, n, lower, upper);
    cost(n, centre, m);

    point *boat = get_point(n);
    boat->f = INFINITY;
    int runs = 0, evaluations = 1;
    do {
        runs++;
        cost(n, centre, m);
        evaluations++;

        if (centre->f < boat->f) {
            copy_point(n, centre, boat);
        }

        set_random_coordinates(centre, n, lower, upper);
    } while (evaluations < max_evaluations);

    fprintf(stderr, "%8d %8d  ", runs, evaluations);
    print_result(n, boat, places, fmt);

    return 0;
}
