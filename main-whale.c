#include <stdio.h>
#include <stdlib.h>
#include "whale.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // options
    options o = get_options(argv, false);
    real minx = strtold(argv[6], NULL);
    real maxx = strtold(argv[7], NULL);

    // model parameters
    model *m = model_init();

    point *solution = get_whale(o.dim, minx, maxx, m);
    whale_algorithm(solution, minx, maxx, m, o);

    fprintf(stderr, "    %s[%s", GRY, NRM);
    for (int k = 0; k < o.dim; k++) {
        fprintf(stderr, "% .6Lf ", solution->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, "  % .6Lf\n", solution->f);

    return 0;
}
