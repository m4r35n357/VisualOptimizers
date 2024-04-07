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
    fprintf(stderr, "%s[%s ", GRY, NRM);
    for (int i = 0; i < c.n; i++) {
        fprintf(stderr, c.fmt ? "% .*Le " : "% .*Lf ", c.places, s->x_star->x[i]);
    }
    fprintf(stderr, c.fmt ? "%s]%s % .*Le\n" : "%s]%s % .*Lf\n", GRY, NRM, c.places, s->x_star->f);

    return 0;
}
