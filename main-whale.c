#include <stdio.h>
#include <stdlib.h>
#include "whale.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // options
    options o = get_options(argv, false);
    real min = strtold(argv[6], NULL);
    real max = strtold(argv[7], NULL);

    // model parameters
    model *m = model_init();

    population *p = get_population(min, max, m, o);

    point *solution = get_whale(o.dim, min, max, m);
    woa(p, solution, min, max, m, o);

    fprintf(stderr, "    %s[%s", GRY, NRM);
    for (int k = 0; k < o.dim; k++) {
        fprintf(stderr, "% .6Lf ", solution->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, "  % .6Lf\n", solution->f);

    return 0;
}
