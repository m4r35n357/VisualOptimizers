#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h"

real A = 48.0L;

static real z(real x, real y) {
    return (A - 2.0L * x * y) / (2.0L * (x + y));
}

real cost (real *position, int dim) { (void)dim;
    real x = position[0];
    real y = position[1];
    return A / (x * y * z(x, y));
}

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 4);

    int dim = (int)strtol(argv[1], NULL, BASE);         CHECK(dim >= 1 && dim <= 100);
    int num_whales = (int)strtol(argv[2], NULL, BASE);  CHECK(num_whales >= 1 && num_whales <= 10000);
    int max_iter = (int)strtol(argv[3], NULL, BASE);    CHECK(max_iter >= 1 && max_iter <= 10000);
    real min_edge = 0.001L;

    real minx = min_edge;
    real maxx = sqrtl(0.5L * A) - min_edge;
    point *solution = create_whale(dim, minx, maxx);
    woa(solution, max_iter, num_whales, dim, minx, maxx);

    fprintf(stderr, "Value: % .6Lf  Points:\n", cost(solution->x, dim));
    for (int k = 0; k < dim; k++) {
    	fprintf(stdout, "% .6Lf ", solution->x[k]);
    }
    fprintf(stdout, "\n");

    return 0;
}
