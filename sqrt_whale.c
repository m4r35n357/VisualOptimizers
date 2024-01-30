#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h" // Assuming "whale.h" is the C header file for the whale optimization algorithm

double cost (double *position, int length) {
    double value = 0.0;
    for (int j = 0; j < length; j++) {
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
        dim = atoi(argv[1]);
        num_whales = atoi(argv[2]);
        max_iter = atoi(argv[3]);
    }

    double *best_x = woa(max_iter, num_whales, dim, -10.0, 10.0);
    printf("Best solution found:\n");
    for (int k = 0; k < dim; k++) {
        printf("%.6f ", best_x[k]);
    }
    printf("\nfitness of best solution = %.6f\n", cost(best_x, dim));
    for (int i = 0; i < dim; i++) {
        printf(" %f", best_x[i]);
    }
    printf("\n");

    free(best_x); // Assuming woa() dynamically allocates memory for best_x
    return 0;
}
