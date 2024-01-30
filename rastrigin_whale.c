#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h"

real cost (real *position, int n) {
    real PI = acos(-1.0L);
    real a = 10.0L;
    real value = a * n;
    for (int j = 0; j < n; j++) {
        real xi = position[j];
        value += xi * xi - a * cosl(2.0L * PI * xi);
    }
    return value;
}

int main(int argc, char *argv[]) {
    int dim, num_whales, max_iter;
    if (argc == 1) {
        dim = 3;
        num_whales = 50;
        max_iter = 100;
    } else {
        dim = (int)strtol(argv[1], NULL, BASE);
        num_whales = (int)strtol(argv[2], NULL, BASE);
        max_iter = (int)strtol(argv[3], NULL, BASE);
    }

    real *best_x = woa(max_iter, num_whales, dim, -10.0L, 10.0L);
    printf("Best solution found:  ");
    for (int k = 0; k < dim; k++) {
        printf("% .6Lf ", best_x[k]);
    }
    printf("  value = % .6Lf\n\n", cost(best_x, dim));

    return 0;
}
