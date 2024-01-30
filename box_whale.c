#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h" // Assuming "whale.h" contains the declaration of the woa function

double A = 48.0;

static double z(double x, double y) {
    return (A - 2.0 * x * y) / (2.0 * (x + y));
}

double cost (double *position, int dim) { (void)dim;
    double x = position[0];
    double y = position[1];
    return A / (x * y * z(x, y));
}

int main(int argc, char *argv[]) {
    int dim, num_whales, max_iter;
    double min_edge = 0.001;
    if (argc == 1) {
        dim = 2;
        num_whales = 50;
        max_iter = 100;
    } else {
        dim = (int)strtol(argv[1], NULL, BASE);
        num_whales = (int)strtol(argv[2], NULL, BASE);
        max_iter = (int)strtol(argv[3], NULL, BASE);
    }

    double *best_x = woa(max_iter, num_whales, dim, min_edge, sqrt(0.5 * A) - min_edge);
    printf("Best solution found:  ");
    for (int k = 0; k < dim; k++) {
        printf("% .6f ", best_x[k]);
    }
    printf("  value = % .6f\n\n", cost(best_x, dim));

    return 0;
}
