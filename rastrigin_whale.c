#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Assuming the 'whale' library and 'woa' function are implemented in C
// and available to include. If not, the corresponding C implementation
// needs to be provided.
#include "whale.h"

static double rastrigin(double *position, int n) {
    double PI = acos(-1.0);
    double a = 10.0;
    double value = a * n;
    for (int j = 0; j < n; j++) {
        double xi = position[j];
        value += xi * xi - a * cos(2.0 * PI * xi);
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

    // Assuming the 'woa' function returns a pointer to the best solution found
    double *best_x = woa(rastrigin, max_iter, num_whales, dim, -10.0, 10.0);

    printf("Best solution found:\n");
    for (int k = 0; k < dim; k++) {
        printf("%.6f ", best_x[k]);
    }
    printf("\nfitness of best solution = %.6f\n", rastrigin(best_x, dim));

    for (int i = 0; i < dim; i++) {
        printf("%f ", best_x[i]);
    }
    printf("\n");

    // Assuming the 'woa' function allocates memory for the best solution
    // If it does, we need to free it
    free(best_x);

    return 0;
}
