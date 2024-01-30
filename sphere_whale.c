#include <stdio.h>
#include <stdlib.h>
#include "whale.h" // Assuming whale.h contains the declaration of woa function

static double sphere(double *position, int length) {
    double value = 0.0;
    for (int j = 0; j < length; j++) {
        double xi = position[j];
        value += xi * xi;
    }
    return value;
}

int main(int argc, char *argv[]) {
    int dim, num_whales, max_iter;
    double *best_x;
    double fitness;

    if (argc == 1) {
        dim = 3;
        num_whales = 50;
        max_iter = 100;
    } else {
        dim = atoi(argv[1]);
        num_whales = atoi(argv[2]);
        max_iter = atoi(argv[3]);
    }

    best_x = woa(sphere, max_iter, num_whales, dim, -10.0, 10.0);

    printf("Best solution found:\n");
    for (int k = 0; k < dim; k++) {
        printf("%.6f ", best_x[k]);
    }
    printf("\n");

    fitness = sphere(best_x, dim);
    printf("fitness of best solution = %.6f\n", fitness);

    for (int i = 0; i < dim; i++) {
        printf(" %f", best_x[i]);
    }
    printf("\n");

    free(best_x); // Assuming woa function allocates memory for best_x

    return 0;
}
