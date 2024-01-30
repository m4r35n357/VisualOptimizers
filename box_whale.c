#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h" // Assuming "whale.h" contains the declaration of the woa function

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
    int dim, num_whales, max_iter;
    real min_edge = 0.001L;
    if (argc == 1) {
        dim = 2;
        num_whales = 50;
        max_iter = 100;
    } else {
        dim = (int)strtol(argv[1], NULL, BASE);
        num_whales = (int)strtol(argv[2], NULL, BASE);
        max_iter = (int)strtol(argv[3], NULL, BASE);
    }

    real *best_x = woa(max_iter, num_whales, dim, min_edge, sqrtl(0.5L * A) - min_edge);
    printf("Best solution found:  ");
    for (int k = 0; k < dim; k++) {
        printf("% .6Lf ", best_x[k]);
    }
    printf("  value = % .6Lf\n\n", cost(best_x, dim));

    return 0;
}
