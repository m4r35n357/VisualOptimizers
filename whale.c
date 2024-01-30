#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include <time.h>
#include "whale.h"

int randint (int n) {
    return (int)((double)rand() / ((double)RAND_MAX + 1) * n);
}

whale *create_whale (int dim, double min_x, double max_x, int seed, double (*cost)(double *, int)) {
    whale *w = (whale *)malloc(sizeof(whale));
    w->x = malloc((size_t)dim * sizeof(double));
    for (int j = 0; j < dim; ++j) {
        w->x[j] = ((max_x - min_x) * ((double)rand() / (double)RAND_MAX) + min_x);
    }
    w->value = cost(w->x, dim);
    return w;
}

double *woa (int max_i, int n, int dim, double min_x, double max_x) {
    whale **whales = malloc((size_t)n * sizeof(whale *));
    for (int i = 0; i < n; ++i) {
        whales[i] = create_whale(dim, min_x, max_x, i, cost);
    }
    double *Xp = malloc((size_t)dim * sizeof(double));
    double *X_next = malloc((size_t)dim * sizeof(double));
    double f_best = DBL_MAX;
    for (int i = 0; i < n; ++i) {
        if (whales[i]->value < f_best) {
            f_best = whales[i]->value;
            for (int j = 0; j < dim; ++j) {
                Xp[j] = whales[i]->x[j];
            }
        }
    }
    srand((unsigned int)time(NULL));
    double PI = acos(-1.0);
    int iteration = 0;
    while (iteration < max_i) {
        if (iteration % 10 == 0 && iteration > 1) {
            printf("Iter = %d best fitness = %.6f\n", iteration, f_best);
        }
        double a = 2.0 * (1.0 - ((double)iteration / max_i));
        for (int i = 0; i < n; ++i) {
            double A = a * (2.0 * ((double)rand() / (double)RAND_MAX) - 1.0);
            double C = 2.0 * ((double)rand() / (double)RAND_MAX);
            double b = 1.0;
            double l = 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;
            if (((double)rand() / (double)RAND_MAX) < 0.5) {
                if (fabs(A) < 1.0) {
                    for (int j = 0; j < dim; ++j) {
                        X_next[j] = Xp[j] - A * fabs(C * Xp[j] - whales[i]->x[j]);
                    }
                } else {
                    int p = randint(n);
                    while (p == i) {
                        p = randint(n);
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
    return Xp;
}
