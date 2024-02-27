#include <stdio.h>
#include <stdlib.h>
#include "spiral.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // options
    config c = get_config(argv, false);

    // model parameters
    model *m = model_init();

    spiral *s = get_spiral(m, c);
    soa(s, m, c);

    fprintf(stderr, "  %5d %6d ", s->k, s->evaluations);
    fprintf(stderr, " %s[%s ", GRY, NRM);
    for (int k = 0; k < c.n; k++) {
        fprintf(stderr, c.fmt ? "% .*Le " : "% .*Lf ", c.places, s->centre->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, c.fmt ? " % .*Le\n" : " % .*Lf\n", c.places, s->centre->f);

    return 0;
}
