#include <stdio.h>
#include <stdlib.h>
#include "spiral.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    config c = get_config(argv, false);
    real min = strtold(argv[6], NULL);
    real max = strtold(argv[7], NULL);

    // model parameters
    model *m = model_init();

    spiral *p = get_spiral(min, max, m, c);
    point *solution = get_point(c.n, min, max, m);
    soa(p, solution, min, max, m, c);

    fprintf(stderr, "    %s[%s", GRY, NRM);
    for (int k = 0; k < c.n; k++) {
        fprintf(stderr, c.fmt ? "% .*Le " : "% .*Lf ", c.places, solution->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, "  % .*Lf\n", c.places, solution->f);
    return 0;
}
