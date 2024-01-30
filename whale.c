#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include <time.h>
#include "whale.h"

int randint (int n) {
    return (int)((real)rand() / ((real)RAND_MAX + 1) * n);
}

real randreal () {
    return (real)rand() / (real)RAND_MAX;
}

whale *create_whale (int dim, real min_x, real max_x) {
    whale *w = malloc(sizeof(whale));
    w->x = malloc((size_t)dim * sizeof(real));
    for (int j = 0; j < dim; ++j) {
        w->x[j] = (max_x - min_x) * randreal() + min_x;
    }
    w->f = cost(w->x, dim);
    return w;
}

real *woa (int max_i, int n, int dim, real min_x, real max_x) {
    srand((unsigned int)time(NULL));
    whale **whales = malloc((size_t)n * sizeof(whale *));
    for (int i = 0; i < n; ++i) {
        whales[i] = create_whale(dim, min_x, max_x);
    }
    real *Xp = malloc((size_t)dim * sizeof(real));
    real *X_next = malloc((size_t)dim * sizeof(real));
    real f_best = DBL_MAX;
    for (int i = 0; i < n; ++i) {
        if (whales[i]->f < f_best) {
            f_best = whales[i]->f;
            for (int j = 0; j < dim; ++j) {
                Xp[j] = whales[i]->x[j];
            }
        }
    }
    real PI = acosl(-1.0L);
    int iteration = 0;
    while (iteration < max_i) {
        if (iteration % 10 == 0 && iteration > 1) printf("Iteration = %d minimum = %.6Lf\n", iteration, f_best);
        real a = 2.0L * (1.0L - (real)iteration / max_i);
        for (int i = 0; i < n; ++i) {
            real A = a * (2.0L * randreal() - 1.0L);
            real C = 2.0L * randreal();
            real b = 1.0L;
            real l = 2.0L * randreal() - 1.0L;
            if (randreal() < 0.5L) {
                if (fabsl(A) < 1.0L) {
                    for (int j = 0; j < dim; ++j) {
                        X_next[j] = Xp[j] - A * fabsl(C * Xp[j] - whales[i]->x[j]);
                    }
                } else {
                    int p = randint(n);
                    while (p == i) p = randint(n);
                    real *Xr = whales[p]->x;
                    for (int j = 0; j < dim; ++j) {
                        X_next[j] = Xr[j] - A * fabsl(C * Xr[j] - whales[i]->x[j]);
                    }
                }
            } else {
                for (int j = 0; j < dim; ++j) {
                    X_next[j] = fabsl(Xp[j] - whales[i]->x[j]) * expl(b * l) * cosl(2.0L * PI * l) + Xp[j];
                }
            }
            for (int j = 0; j < dim; ++j) {
                whales[i]->x[j] = X_next[j];
            }
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < dim; ++j) {
                whales[i]->x[j] = fmaxl(whales[i]->x[j], min_x);
                whales[i]->x[j] = fminl(whales[i]->x[j], max_x);
            }
            whales[i]->f = cost(whales[i]->x, dim);
            if (whales[i]->f < f_best) {
                for (int j = 0; j < dim; ++j) {
                    Xp[j] = whales[i]->x[j];
                }
                f_best = whales[i]->f;
            }
        }
        iteration++;
    }
    return Xp;
}
