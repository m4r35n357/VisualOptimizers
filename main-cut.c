#include <stdio.h>
#include <stdlib.h>
#include "cut.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // options
    config c = get_config(argv, false);

    // model parameters
    model *m = model_init();

    box *b = get_box(m, c);

    // run optimization
    coa(b, m, c);

    // print solution
    fprintf(stderr, "  %5d %6d  ", b->iterations, b->evaluations);
    fprintf(stderr, "%s[%s ", GRY, NRM);
    for (int i = 0; i < c.n; i++) {
        fprintf(stderr, c.fmt ? "% .*Le " : "% .*Lf ", c.places, b->best->x[i]);
    }
    fprintf(stderr, c.fmt ? "%s]%s % .*Le\n" : "%s]%s % .*Lf\n", GRY, NRM, c.places, b->best->f);

    return 0;
}