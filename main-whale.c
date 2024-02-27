#include <stdio.h>
#include <stdlib.h>
#include "whale.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // options
    options o = get_options(argv, false);

    // model parameters
    model *m = model_init();

    population *p = get_population(m, o);

    woa(p, m, o);

    fprintf(stderr, "  %5d %6d ", p->iterations, p->evaluations);
    fprintf(stderr, " %s[%s ", GRY, NRM);
    for (int k = 0; k < o.dim; k++) {
        fprintf(stderr, o.fmt ? "% .*Le " : "% .*Lf ", o.places, p->prey->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, o.fmt ? " % .*Le\n" : " % .*Lf\n", o.places, p->prey->f);

    return 0;
}
