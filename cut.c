#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cut.h"

config get_config (char **argv, bool single) {
    unsigned int seed = 0;
    FILE *devrnd = fopen("/dev/urandom","r");  CHECK(devrnd);
    fread(&seed, 4, 1, devrnd);
    int opened = fclose(devrnd);  CHECK(!opened);
    srand(seed);
    config conf = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .m = (int)strtol(argv[4], NULL, BASE),
        .max_iterations = (int)strtol(argv[5], NULL, BASE),
        .lower = strtold(argv[6], NULL),
        .upper = strtold(argv[7], NULL),
        .step_mode = single
    };
    CHECK(conf.places >= 1 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 64);
    CHECK(conf.m >= 1 && conf.m <= 100000);
    CHECK(conf.max_iterations >= 1 && conf.max_iterations <= 1000);
    CHECK(conf.upper > conf.lower);
    return conf;
}

static real rand_range (real lower, real upper) {
    return (upper - lower) * (real)rand() / (real)RAND_MAX + lower;
}

box *get_box (model *m, config c) {
    box *b =  malloc(sizeof(box));
    b->upper = malloc((size_t)c.n * sizeof (real)); CHECK(b->upper);
    b->lower = malloc((size_t)c.n * sizeof (real)); CHECK(b->lower);
    for (int k = 0; k < c.n; k++) {
        b->upper[k] = c.upper;
        b->lower[k] = c.lower;
    }
    b->iterations = b->evaluations = 0;
    b->p = malloc((size_t)c.m * sizeof (point *));    CHECK(b->p);
    for (int i = 0; i < c.m; i++) {
        b->p[i] = malloc(sizeof (point));          CHECK(b->p[i]);
        b->p[i]->x = malloc((size_t)c.n * sizeof (real)); CHECK(b->p[i]->x);
        for (int k = 0; k < c.n; k++) {
            b->p[i]->x[k] = rand_range(c.lower, c.upper);
        }
        cost(c.n, b->p[i], m);
        b->evaluations++;
    }
    b->best = b->p[0];
    for (int i = 1; i < c.m; i++) {
        if (b->p[i]->f < b->best->f) b->best = b->p[i];
    }
    b->looping = false;
    b->lambda = powl(0.1L / c.max_iterations, 1.0L / c.max_iterations);
    return b;
}

bool coa (box *b, model *m, config c) {
    if (c.step_mode && b->looping) goto resume; else b->looping = true;
    while (b->iterations < c.max_iterations) {
        for (int k = 0; k < c.n; k++) {
            real lower, upper, side = 0.5L * b->lambda * (b->upper[k] - b->lower[k]);
            upper = b->best->x[k] + side;
            lower = b->best->x[k] - side;
            if (lower < b->lower[k]) {
                upper += b->lower[k] - lower;
                lower = b->lower[k];
            } else if (upper > b->upper[k]) {
                lower += b->upper[k] - upper;
                upper = b->upper[k];
            }
            b->upper[k] = upper;
            b->lower[k] = lower;
        }
        for (int i = 0; i < c.m; i++) {
            if (b->p[i] != b->best) {
                for (int k = 0; k < c.n; k++) {
                    b->p[i]->x[k] = rand_range(b->lower[k], b->upper[k]);
                }
                cost(c.n, b->p[i], m);
                b->evaluations++;
                if (b->p[i]->f < b->best->f) {
                	b->best = b->p[i];
                    printf("  %5d %6d  [ ", b->iterations + 1, b->evaluations);
                    for (int k = 0; k < c.n; k++) {
                        printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, b->best->x[k]);
                    }
                    printf(c.fmt ? "] % .*Le\n" : "] % .*Lf\n", c.places, b->best->f);
                }
            }
        }
        b->iterations++;
        if (c.step_mode) return true;
        resume: ;
    }
    return b->looping = false;
}
