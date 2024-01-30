#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h" // Assuming "whale.h" is the C header file for the whale optimization algorithm

real cost (real *position, int dim) {
    real value = 0.0L;
    for (int j = 0; j < dim; j++) {
        real xi = position[j];
        value += sqrtl(fabsl(xi));
    }
    return value;
}

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
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

    real minx = -10.0L;
    real maxx = 10.0L;
    point *solution = create_whale(dim, minx, maxx);
    woa(solution, max_iter, num_whales, dim, minx, maxx);
    printf("Best solution found:  ");
    for (int k = 0; k < dim; k++) {
        printf("% .6Lf ", solution->x[k]);
    }
    printf("  value = % .6Lf\n\n", cost(solution->x, dim));

    return 0;
}
