#include <stdio.h>
#include <stdlib.h>
#include "whale.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 6);

    int dim = (int)strtol(argv[1], NULL, BASE);         CHECK(dim >= 1 && dim <= 100);
    int num_whales = (int)strtol(argv[2], NULL, BASE);  CHECK(num_whales >= 1 && num_whales <= 10000);
    int max_iter = (int)strtol(argv[3], NULL, BASE);    CHECK(max_iter >= 1 && max_iter <= 10000);
    real minx = strtold(argv[4], NULL);
    real maxx = strtold(argv[5], NULL);

    model *m = model_init();

    point *solution = create_whale(dim, minx, maxx, m);
    woa(solution, max_iter, num_whales, dim, minx, maxx, m);

    fprintf(stderr, "    %s[%s", GRY, NRM);
    for (int k = 0; k < dim; k++) {
    	fprintf(stderr, "% .6Lf ", solution->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, "  % .6Lf\n", solution->f);

    return 0;
}
