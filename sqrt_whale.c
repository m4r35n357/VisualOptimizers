#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h" // Assuming "whale.h" is the C header file for the whale optimization algorithm

double cost (double *position, int dim) {
    double value = 0.0;
    for (int j = 0; j < dim; j++) {
        double xi = position[j];
        value += sqrt(fabs(xi));
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

    double *best_x = woa(max_iter, num_whales, dim, -10.0, 10.0);
    printf("Best solution found:  ");
    for (int k = 0; k < dim; k++) {
        printf("% .6f ", best_x[k]);
    }
    printf("  value = % .6f\n\n", cost(best_x, dim));

    return 0;
}
