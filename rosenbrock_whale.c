#include <stdio.h>
#include <stdlib.h>
#include "whale.h"

real cost (real *position, int dim) {
    real value = 0.0L;
    for (int j = 0; j < dim - 1; j++) {
        real xi = position[j];
        value += (1.0L - xi) * (1.0L - xi) + 100.0L * (position[j + 1] - xi * xi) * (position[j + 1] - xi * xi);
    }
    return value;
}

int main (int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 6);

    int dim = (int)strtol(argv[1], NULL, BASE);         CHECK(dim >= 1 && dim <= 100);
    int num_whales = (int)strtol(argv[2], NULL, BASE);  CHECK(num_whales >= 1 && num_whales <= 10000);
    int max_iter = (int)strtol(argv[3], NULL, BASE);    CHECK(max_iter >= 1 && max_iter <= 10000);
    real minx = strtold(argv[4], NULL);
    real maxx = strtold(argv[5], NULL);

    point *solution = create_whale(dim, minx, maxx);
    woa(solution, max_iter, num_whales, dim, minx, maxx);

    fprintf(stderr, "Value: % .6Lf  Points:\n", cost(solution->x, dim));
    for (int k = 0; k < dim; k++) {
    	fprintf(stdout, "% .6Lf ", solution->x[k]);
    }
    fprintf(stdout, "\n");

    return 0;
}
