#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "whale.h"

options get_options (char **argv, bool single) {
    unsigned int seed = 0;
    FILE *devrnd = fopen("/dev/urandom","r");  CHECK(devrnd);
    fread(&seed, 4, 1, devrnd);
    int opened = fclose(devrnd);  CHECK(!opened);
    srand(seed);
    options opt = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .dim = (int)strtol(argv[3], NULL, BASE),
        .whales = (int)strtol(argv[4], NULL, BASE),
        .iterations = (int)strtol(argv[5], NULL, BASE),
        .lower = strtold(argv[6], NULL),
        .upper = strtold(argv[7], NULL),
        .step_mode = single
    };
    CHECK(opt.places >= 3 && opt.places <= 36);
    CHECK(opt.fmt == 0 || opt.fmt == 1);
    CHECK(opt.dim >= 1 && opt.dim <= 100);
    CHECK(opt.whales >= 1 && opt.whales <= 10000);
    CHECK(opt.iterations >= 1 && opt.iterations <= 100000);
    CHECK(opt.upper > opt.lower);
    return opt;
}

real rand_real () {
    return (real)rand() / (real)RAND_MAX;
}

population *get_population (model *m, options o) {
    population *p =  malloc(sizeof(population));
    p->iterations = p->evaluations = 0;
    p->whales = malloc((size_t)o.whales * sizeof(point *));
    for (int i = 0; i < o.whales; i++) {
        p->whales[i] = malloc(sizeof(point));
        p->whales[i]->x = malloc((size_t)o.dim * sizeof(real));
        for (int j = 0; j < o.dim; j++) {
            p->whales[i]->x[j] = (o.upper - o.lower) * rand_real() + o.lower;
        }
        cost(o.dim, p->whales[i], m);
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

bool woa (population *p, model *m, options o) {
    real TWO_PI = 2.0L * acosl(-1.0L);
    if (o.step_mode && p->looping) goto resume; else p->looping = true;
    while (p->iterations < o.iterations) {
        real a = 2.0L * (1.0L - (real)p->iterations / (real)o.iterations);
        for (int i = 0; i < o.whales; i++) {
            point *current = p->whales[i];
            if (current != p->prey) {
                real A = a * (2.0L * rand_real() - 1.0L);
                real C = 2.0L * rand_real();
                real b = 5.0L * a;
                real l = 2.0L * rand_real() - 1.0L;
                if (rand_real() < 0.5L) {
                    if (fabsl(A) < 1.0L) { // "encircling" update (1)
                        for (int j = 0; j < o.dim; j++) {
                            current->x[j] = p->prey->x[j] - A * fabsl(C * p->prey->x[j] - current->x[j]);
                        }
                    } else {  // "searching/random" update (9)
                        int r; do r = (int)((real)rand() / ((real)RAND_MAX + 1) * o.whales); while (r == i);
                        for (int j = 0; j < o.dim; j++) {
                            current->x[j] = p->whales[r]->x[j] - A * fabsl(C * p->whales[r]->x[j] - current->x[j]);
                        }
                    }
                } else {  // "spiral" update (7)
                    for (int j = 0; j < o.dim; j++) {
                        current->x[j] = fabsl(p->prey->x[j] - current->x[j]) * expl(b * l) * cosl(TWO_PI * l) + p->prey->x[j];
                    }
                }
                bool oor = false;
                for (int j = 0; j < o.dim; j++) {
                    if (current->x[j] > o.upper || current->x[j] < o.lower) {
                        oor = true;
                        break;
                    }
                }
                if (oor) {
                    for (int j = 0; j < o.dim; j++) {
                        current->x[j] = (o.upper - o.lower) * rand_real() + o.lower;
                    }
                }
                cost(o.dim, current, m);
                p->evaluations++;
                if (current->f < p->prey->f) p->prey = current;
            }
        }
        if (++p->iterations % 10 == 0) {
            printf("  %05d %06d  [ ", p->iterations, p->evaluations);
            for (int j = 0; j < o.dim; j++) {
                printf(o.fmt ? "% .*Le " : "% .*Lf ", o.places, p->prey->x[j]);
            }
            printf(o.fmt ? "]  % .*Le\n" : "]  % .*Lf\n", o.places, p->prey->f);
        }
        if (o.step_mode) return true;
        resume: ;
    }
    return p->looping = false;
}
