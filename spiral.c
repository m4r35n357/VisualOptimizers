#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "spiral.h"

config get_config (char **argv, bool single) {
	config conf = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .m = (int)strtol(argv[4], NULL, BASE),
        .k_max = (int)strtol(argv[5], NULL, BASE),
        .step_mode = single
    };
    CHECK(conf.places >= 3 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 100);
    CHECK(conf.m >= 1 && conf.m <= 10000);
    CHECK(conf.k_max >= 1 && conf.k_max <= 10000);
    return conf;
}

int randint (int n) {
    return (int)((real)rand() / ((real)RAND_MAX + 1) * n);
}

real randreal () {
    return (real)rand() / (real)RAND_MAX;
}

whale *get_point (int dim, real min_x, real max_x, model *m) {
    point *p = malloc(sizeof(point));
    p->x = malloc((size_t)dim * sizeof(real));
    for (int j = 0; j < dim; j++) {
        p->x[j] = (max_x - min_x) * randreal() + min_x;
    }
    cost(dim, p, m);
    return p;
}

spiral *get_spiral (real min_x, real max_x, model *m, config c) {
    srand((unsigned int)time(NULL));
    spiral *s =  malloc(sizeof(spiral));
    s->iterations = s->evaluations = 0;
    s->points = malloc((size_t)c.m * sizeof(point *));
    for (int i = 0; i < c.m; i++) {
        s->points[i] = get_point(c.n, min_x, max_x, m);
        s->evaluations++;
    }
    s->evaluations++;
    for (int i = 0; i < c.m; i++) {
        if (s->points[i]->f < s->x_star->f) {
            for (int j = 0; j < c.n; j++) {
                s->x_star->x[j] = s->points[i]->x[j];
            }
            s->x_star->f = s->points[i]->f;
        }
    }
    s->looping = false;
    return s;
}

bool woa (population *p, point *solution, real min_x, real max_x, model *m, options o) {
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
                        p->X_next[j] = p->Xp->x[j] - A * fabsl(C * p->Xp->x[j] - p->whales[i]->x[j]);
                    }
                } else {  // "searching/random" update (9)
                    int r = randint(o.whales);
                    while (r == i) r = randint(o.whales);
                    real *Xr = p->whales[r]->x;
                    for (int j = 0; j < o.dim; j++) {
                        p->X_next[j] = Xr[j] - A * fabsl(C * Xr[j] - p->whales[i]->x[j]);
                    }
                }
            } else {  // "spiral" update (7)
                for (int j = 0; j < o.dim; j++) {
                    p->X_next[j] = fabsl(p->Xp->x[j] - p->whales[i]->x[j]) * expl(b * l) * cosl(TWO_PI * l) + p->Xp->x[j];
                }
            }
            for (int j = 0; j < o.dim; j++) {
                p->whales[i]->x[j] = p->X_next[j];
            }
        }
        for (int i = 0; i < o.whales; i++) {
            for (int j = 0; j < o.dim; j++) {
                p->whales[i]->x[j] = fmaxl(p->whales[i]->x[j], min_x);
                p->whales[i]->x[j] = fminl(p->whales[i]->x[j], max_x);
            }
            cost(o.dim, p->whales[i], m);
            p->evaluations++;
            if (p->whales[i]->f < p->Xp->f) {
                for (int j = 0; j < o.dim; j++) {
                    p->Xp->x[j] = p->whales[i]->x[j];
                }
                p->Xp->f = p->whales[i]->f;
            }
        }
        p->iterations++;
        printf(" %05d %06d  [ ", p->iterations, p->evaluations);
        for (int j = 0; j < o.dim; j++) {
            printf(o.fmt ? "% .*Le " : "% .*Lf ", o.places, p->Xp->x[j]);
        }
        printf(o.fmt ? "]  % .*Le\n" : "]  % .*Lf\n", o.places, p->Xp->f);
        if (o.step_mode) return true;
        resume: ;
    }
    for (int i = 0; i < o.whales; i++) {
        for (int j = 0; j < o.dim; j++) {
            solution->x[j] = p->Xp->x[j];
        }
        solution->f = p->Xp->f;
    }
    return p->looping = false;
}
