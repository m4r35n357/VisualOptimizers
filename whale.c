#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "whale.h"

options get_options (char **argv, bool single) {
    options opt = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .dim = (int)strtol(argv[3], NULL, BASE),
        .whales = (int)strtol(argv[4], NULL, BASE),
        .iterations = (int)strtol(argv[5], NULL, BASE),
        .step_mode = single
    };
    CHECK(opt.places >= 3 && opt.places <= 36);
    CHECK(opt.fmt == 0 || opt.fmt == 1);
    CHECK(opt.dim >= 1 && opt.dim <= 100);
    CHECK(opt.whales >= 1 && opt.whales <= 10000);
    CHECK(opt.iterations >= 1 && opt.iterations <= 10000);
    return opt;
}

int randint (int n) {
    return (int)((real)rand() / ((real)RAND_MAX + 1) * n);
}

real randreal () {
    return (real)rand() / (real)RAND_MAX;
}

whale *get_whale (int dim, real min_x, real max_x, model *m) {
    whale *w = malloc(sizeof(whale));
    w->x = malloc((size_t)dim * sizeof(real));
    for (int j = 0; j < dim; j++) {
        w->x[j] = (max_x - min_x) * randreal() + min_x;
    }
    cost(dim, w, m);
    return w;
}

population *get_population (real min_x, real max_x, model *m, options o) {
    srand((unsigned int)time(NULL));
    population *p =  malloc(sizeof(population));
    p->iterations = p->evaluations = 0;
    p->whales = malloc((size_t)o.whales * sizeof(whale *));
    for (int i = 0; i < o.whales; i++) {
        p->whales[i] = get_whale(o.dim, min_x, max_x, m);
        p->evaluations++;
    }
    p->prey = p->whales[0];
    for (int i = 1; i < o.whales; i++) {
        if (p->whales[i]->f < p->prey->f) {
            p->prey = p->whales[i];
        }
    }
    p->looping = false;
    return p;
}

bool woa (population *p, real min_x, real max_x, model *m, options o) {
    real TWO_PI = 2.0L * acosl(-1.0L);
    if (o.step_mode && p->looping) goto resume; else p->looping = true;
    while (p->iterations < o.iterations) {
        real a = 2.0L * (1.0L - (real)p->iterations / (real)o.iterations);
        for (int i = 0; i < o.whales; i++) {
            real A = a * (2.0L * randreal() - 1.0L);
            real C = 2.0L * randreal();
            real b = 1.0L;
            real l = 2.0L * randreal() - 1.0L;
            if (randreal() < 0.5L) {
                if (fabsl(A) < 1.0L) { // "encircling" update (1)
                    for (int j = 0; j < o.dim; j++) {
                        p->whales[i]->x[j] = p->prey->x[j] - A * fabsl(C * p->prey->x[j] - p->whales[i]->x[j]);
                    }
                } else {  // "searching/random" update (9)
                    int r = randint(o.whales);
                    while (r == i) r = randint(o.whales);
                    real *Xr = p->whales[r]->x;
                    for (int j = 0; j < o.dim; j++) {
                        p->whales[i]->x[j] = Xr[j] - A * fabsl(C * Xr[j] - p->whales[i]->x[j]);
                    }
                }
            } else {  // "spiral" update (7)
                for (int j = 0; j < o.dim; j++) {
                    p->whales[i]->x[j] = fabsl(p->prey->x[j] - p->whales[i]->x[j]) * expl(b * l) * cosl(TWO_PI * l) + p->prey->x[j];
                }
            }
        }
        for (int i = 0; i < o.whales; i++) {
            for (int j = 0; j < o.dim; j++) {
                p->whales[i]->x[j] = fmaxl(p->whales[i]->x[j], min_x);
                p->whales[i]->x[j] = fminl(p->whales[i]->x[j], max_x);
            }
            cost(o.dim, p->whales[i], m);
            p->evaluations++;
            if (p->whales[i]->f < p->prey->f) {
                p->prey = p->whales[i];
            }
        }
        p->iterations++;
        printf(" %05d %06d  [ ", p->iterations, p->evaluations);
        for (int j = 0; j < o.dim; j++) {
            printf(o.fmt ? "% .*Le " : "% .*Lf ", o.places, p->prey->x[j]);
        }
        printf(o.fmt ? "]  % .*Le\n" : "]  % .*Lf\n", o.places, p->prey->f);
        if (o.step_mode) return true;
        resume: ;
    }
    return p->looping = false;
}
