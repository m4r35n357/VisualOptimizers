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
        .k_max = (int)strtol(argv[5], NULL, BASE),
        .lower = strtold(argv[6], NULL),
        .upper = strtold(argv[7], NULL),
        .step_mode = single
    };
    CHECK(conf.places >= 1 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 100);
    CHECK(conf.m >= 1 && conf.m <= 10000);
    CHECK(conf.k_max >= 1 && conf.k_max <= 100000);
    CHECK(conf.upper > conf.lower);
    return conf;
}

static real rand_range (real lower, real upper) {
    return (upper - lower) * (real)rand() / (real)RAND_MAX + lower;
}

box *get_box (model *m, config c) {
    box *b =  malloc(sizeof(box));
    b->x_u = malloc((size_t)c.n * sizeof (real)); CHECK(b->x_u);
    b->x_l = malloc((size_t)c.n * sizeof (real)); CHECK(b->x_l);
    for (int k = 0; k < c.n; k++) {
    	b->x_u[k] = c.upper;
    	b->x_l[k] = c.lower;
    }
    b->k = b->evaluations = 0;
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
    b->looping = b->dual_mode = false;
    b->lambda = powl(1.0L / c.k_max, 1.0L / c.k_max);
    return b;
}

bool coa (box *b, model *m, config c) {
    if (c.step_mode && b->looping) goto resume; else b->looping = true;
    while (b->k < c.k_max) {
        for (int k = 0; k < c.n; k++) {
        	real lower, upper, side = 0.5L * b->lambda * (b->x_u[k] - b->x_l[k]);
        	upper = b->best->x[k] + side;
        	lower = b->best->x[k] - side;
            if (lower < b->x_l[k]) {
            	upper += b->x_l[k] - lower;
            	lower = b->x_l[k];
            } else if (upper > b->x_u[k]) {
            	lower += b->x_u[k] - upper;
            	upper = b->x_u[k];
            }
        	b->x_u[k] = upper;
        	b->x_l[k] = lower;
        }
        for (int i = 0; i < c.m; i++) {
            if (b->p[i] != b->best) {
                for (int k = 0; k < c.n; k++) {
                    b->p[i]->x[k] = rand_range(b->x_l[k], b->x_u[k]);
                }
                cost(c.n, b->p[i], m);
                b->evaluations++;
                if (b->p[i]->f < b->best->f) b->best = b->p[i];
            }
        }
        if (++b->k % 10 == 0) {
            printf("  %5d %6d  [ ", b->k, b->evaluations);
            for (int k = 0; k < c.n; k++) {
                printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, b->best->x[k]);
            }
            printf(c.fmt ? "] % .*Le\n" : "] % .*Lf\n", c.places, b->best->f);
        }
        if (c.step_mode) return true;
        resume: ;
    }
    return b->looping = false;
}
