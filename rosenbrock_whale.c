#include <stdio.h>
#include <stdlib.h>

// Assuming the 'whale' library and 'woa' function are available in C
// and have been properly translated and adapted.
#include "whale.h"

double rosenbrock(double *position, int dim) {
    double value = 0.0;
    for (int j = 0; j < dim - 1; j++) {
        double xi = position[j];
        value += (1.0 - xi) * (1.0 - xi) + 100.0 * (position[j + 1] - xi * xi) * (position[j + 1] - xi * xi);
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
        dim = atoi(argv[1]);
        num_whales = atoi(argv[2]);
        max_iter = atoi(argv[3]);
    }

    double *best_x = woa(rosenbrock, max_iter, num_whales, dim, -10.0, 10.0);
    printf("Best solution found:\n");
    for (int k = 0; k < dim; k++) {
        printf("%.6f ", best_x[k]);
    }
    printf("\nfitness of best solution = %.6f\n", rosenbrock(best_x, dim));
    for (int i = 0; i < dim; i++) {
        printf(" %f", best_x[i]);
    }
    printf("\n");

    // Assuming dynamic allocation was used in woa, we need to free the memory
    free(best_x);

    return 0;
}
