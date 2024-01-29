#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h" // Assuming "whale.h" contains the declaration of the woa function

double A = 48.0;

double z(double x, double y) {
    return (A - 2.0 * x * y) / (2.0 * (x + y));
}

double box(double position[2]) {
    double x = position[0];
    double y = position[1];
    return A / (x * y * z(x, y));
}

int main(int argc, char *argv[]) {
    int num_whales, max_iter;
    double min_edge = 0.001;
    double best_x[2];

    if (argc == 1) {
        num_whales = 50;
        max_iter = 100;
    } else {
        num_whales = atoi(argv[1]);
        max_iter = atoi(argv[2]);
    }

    woa(box, max_iter, num_whales, 2, min_edge, sqrt(0.5 * A) - min_edge, best_x);

    printf("Best solution found:\n");
    printf("x %.6f  y %.6f  z %.6f ", best_x[0], best_x[1], z(best_x[0], best_x[1]));
    printf("V %.6f\n", best_x[0] * best_x[1] * z(best_x[0], best_x[1]));
    printf("fitness of best solution = %.6f\n", box(best_x));

    return 0;
}
