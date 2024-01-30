#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "whale.h"

int randint (int n) {
    return (int)((real)rand() / ((real)RAND_MAX + 1) * n);
}

real randreal () {
    return (real)rand() / (real)RAND_MAX;
}

whale *create_whale (int dim, real min_x, real max_x, model *m) {
    whale *w = malloc(sizeof(whale));
    w->x = malloc((size_t)dim * sizeof(real));
    for (int j = 0; j < dim; ++j) {
        w->x[j] = (max_x - min_x) * randreal() + min_x;
    }
    cost(dim, w, m);
    return w;
}

bool woa (point *Xp, int max_i, int n_whales, int n_dim, real min_x, real max_x, model *m) {
    srand((unsigned int)time(NULL));
    whale **whales = malloc((size_t)n_whales * sizeof(whale *));
    for (int i = 0; i < n_whales; ++i) {
        whales[i] = create_whale(n_dim, min_x, max_x, m);
    }
    real *X_next = malloc((size_t)n_dim * sizeof(real));
    Xp->f = DBL_MAX;
    for (int i = 0; i < n_whales; ++i) {
        if (whales[i]->f < Xp->f) {
        	Xp->f = whales[i]->f;
            for (int j = 0; j < n_dim; ++j) {
            	Xp->x[j] = whales[i]->x[j];
            }
        }
    }
    real PI = acosl(-1.0L);
    int iteration = 0;
    while (iteration < max_i) {
        if (iteration % 10 == 0 && iteration > 1) fprintf(stdout, "Iteration = %d minimum = %.6Lf\n", iteration, Xp->f);
        real a = 2.0L * (1.0L - (real)iteration / max_i);
        for (int i = 0; i < n_whales; ++i) {
            real A = a * (2.0L * randreal() - 1.0L);
            real C = 2.0L * randreal();
            real b = 1.0L;
            real l = 2.0L * randreal() - 1.0L;
            if (randreal() < 0.5L) {
                if (fabsl(A) < 1.0L) {
                    for (int j = 0; j < n_dim; ++j) {
                        X_next[j] = Xp->x[j] - A * fabsl(C * Xp->x[j] - whales[i]->x[j]);
                    }
                } else {
                    int p = randint(n_whales);
                    while (p == i) p = randint(n_whales);
                    real *Xr = whales[p]->x;
                    for (int j = 0; j < n_dim; ++j) {
                        X_next[j] = Xr[j] - A * fabsl(C * Xr[j] - whales[i]->x[j]);
                    }
                }
            } else {
                for (int j = 0; j < n_dim; ++j) {
                    X_next[j] = fabsl(Xp->x[j] - whales[i]->x[j]) * expl(b * l) * cosl(2.0L * PI * l) + Xp->x[j];
                }
            }
            for (int j = 0; j < n_dim; ++j) {
                whales[i]->x[j] = X_next[j];
            }
        }
        for (int i = 0; i < n_whales; ++i) {
            for (int j = 0; j < n_dim; ++j) {
                whales[i]->x[j] = fmaxl(whales[i]->x[j], min_x);
                whales[i]->x[j] = fminl(whales[i]->x[j], max_x);
            }
            cost(n_dim, whales[i], m);
            if (whales[i]->f < Xp->f) {
                for (int j = 0; j < n_dim; ++j) {
                    Xp->x[j] = whales[i]->x[j];
                }
                Xp->f = whales[i]->f;
            }
        }
        iteration++;
    }
    return Xp;
}
