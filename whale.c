#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "whale.h"

whale *create_whale(int dim, double min_x, double max_x, int seed, double (*cost)(double *, int)) {
    whale *w = (whale *)malloc(sizeof(whale));
    w->x = malloc((size_t)dim * sizeof(double));
    srand((unsigned int)seed + 1);
    for (int j = 0; j < dim; ++j) {
        w->x[j] = ((max_x - min_x) * ((double)rand() / RAND_MAX) + min_x);
    }
    w->value = cost(w->x, dim);
    return w;
}

void free_whale(whale *w) {
    free(w->x);
    free(w);
}

double *woa(double (*cost)(double *, int), int max_i, int n, int dim, double min_x, double max_x) {
    double PI = acos(-1.0);
    srand(0);
    whale **whales = malloc((size_t)n * sizeof(whale *));
    for (int i = 0; i < n; ++i) {
        whales[i] = create_whale(dim, min_x, max_x, i, cost);
    }
    double *Xp = malloc((size_t)dim * sizeof(double));
    double f_best = DBL_MAX;
    for (int i = 0; i < n; ++i) {
        if (whales[i]->value < f_best) {
            f_best = whales[i]->value;
            for (int j = 0; j < dim; ++j) {
                Xp[j] = whales[i]->x[j];
            }
        }
    }
    int iteration = 0;
    while (iteration < max_i) {
        if (iteration % 10 == 0 && iteration > 1) {
            printf("Iter = %d best fitness = %.6f\n", iteration, f_best);
        }
        double a = 2.0 * (1.0 - ((double)iteration / max_i));
        for (int i = 0; i < n; ++i) {
            double A = a * (2.0 * ((double)rand() / RAND_MAX) - 1.0);
            double C = 2.0 * ((double)rand() / RAND_MAX);
            double b = 1.0;
            double l = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
            double *X_next = malloc((size_t)dim * sizeof(double));
            if ((double)rand() / RAND_MAX < 0.5) {
                if (fabs(A) < 1.0) {
                    for (int j = 0; j < dim; ++j) {
                        X_next[j] = Xp[j] - A * fabs(C * Xp[j] - whales[i]->x[j]);
                    }
                } else {
                    int p = rand() % n;
                    while (p == i) {
                        p = rand() % n;
                    }
                    double *Xr = whales[p]->x;
                    for (int j = 0; j < dim; ++j) {
                        X_next[j] = Xr[j] - A * fabs(C * Xr[j] - whales[i]->x[j]);
                    }
                }
            } else {
                for (int j = 0; j < dim; ++j) {
                    X_next[j] = fabs(Xp[j] - whales[i]->x[j]) * exp(b * l) * cos(2.0 * PI * l) + Xp[j];
                }
            }
            for (int j = 0; j < dim; ++j) {
                whales[i]->x[j] = X_next[j];
            }
            free(X_next);
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < dim; ++j) {
                whales[i]->x[j] = fmax(whales[i]->x[j], min_x);
                whales[i]->x[j] = fmin(whales[i]->x[j], max_x);
            }
            whales[i]->value = cost(whales[i]->x, dim);
            if (whales[i]->value < f_best) {
                for (int j = 0; j < dim; ++j) {
                    Xp[j] = whales[i]->x[j];
                }
                f_best = whales[i]->value;
            }
        }
        iteration++;
    }
    for (int i = 0; i < n; ++i) {
        free_whale(whales[i]);
    }
    free(whales);
    return Xp;
}

// Example usage:
// double my_cost_function(double *x, int dim) {
//     // Implement your cost function here
//     return 0.0;
// }
// int main() {
//     int max_i = 100;
//     int n = 30;
//     int dim = 2;
//     double min_x = -5.0;
//     double max_x = 5.0;
//     double *best_solution = woa(my_cost_function, max_i, n, dim, min_x, max_x);
//     // Use best_solution
//     free(best_solution);
//     return 0;
// }
