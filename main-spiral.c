#include <stdio.h>
#include <stdlib.h>
#include "spiral.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    config c = get_config(argv, false);

    // model parameters
    model *m = model_init();

    spiral *s = get_spiral(m, c);

    // run optimization
    soa(s, m, c);

    // print solution
    fprintf(stderr, "  %5d %6d  ", s->k, s->evaluations);
    print_result(c.n, s->x_star, c.places, c.fmt);

    return 0;
}
