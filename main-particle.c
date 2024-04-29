#include <stdio.h>
#include <stdlib.h>
#include "particles.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    config c = get_config(argv);

    // model parameters
    model *m = model_init();

    if (c.spiral) {
        population *s = get_spiral(m, c);

        // run optimization
        soa(s, m, c);

        // print solution
        fprintf(stderr, "  %5d %6d  ", s->iterations, s->evaluations);
        print_result(c.n, s->x_star, c.places, c.fmt);
    } else {
        population *b = get_box(m, c);

        // run optimization
        coa(b, m, c);

        // print solution
        fprintf(stderr, "  %5d %6d  ", b->iterations, b->evaluations);
        print_result(c.n, b->best, c.places, c.fmt);
    }

    return 0;
}
