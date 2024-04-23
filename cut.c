#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cut.h"

config get_config (char **argv, bool single) {
    randomize();
    config conf = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .m = (int)strtol(argv[4], NULL, BASE),
        .max_iterations = (int)strtol(argv[5], NULL, BASE),
        .clamp = (int)strtol(argv[6], NULL, BASE),
        .lower = strtold(argv[7], NULL),
        .upper = strtold(argv[8], NULL),
        .step_mode = single
    };
    CHECK(conf.places >= 1 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 64);
    CHECK(conf.m >= 1 && conf.m <= 100000);
    CHECK(conf.max_iterations >= 1 && conf.max_iterations <= 1000);
    CHECK(conf.clamp == 0 || conf.clamp == 1);
    CHECK(conf.upper > conf.lower);
    return conf;
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
    b->p = malloc((size_t)c.m * sizeof (point *));        CHECK(b->p);
    for (int i = 0; i < c.m; i++) {
        b->p[i] = get_point(c.n); CHECK(b->p[i]);
        set_random_coordinates(b->p[i], c.n, c.lower, c.upper);
        cost(c.n, b->p[i], m);
        b->evaluations++;
    }
    b->best = b->p[0];
    for (int i = 1; i < c.m; i++) {
        if (b->p[i]->f < b->best->f) b->best = b->p[i];
    }
    b->looping = false;
    b->lambda = powl(SHRINK_FACTOR, 1.0L / c.max_iterations);
    return b;
}

bool coa (box *b, model *m, config c) {
    if (c.step_mode && b->looping) goto resume; else b->looping = true;
    while (b->iterations < c.max_iterations) {
        for (int k = 0; k < c.n; k++) {
            real side = 0.5L * b->lambda * (b->upper[k] - b->lower[k]);
            real upper = b->best->x[k] + side;
            real lower = b->best->x[k] - side;
            real lower_limit = c.clamp ? b->lower[k] : c.lower;
            real upper_limit = c.clamp ? b->upper[k] : c.upper;
            if (lower < lower_limit) {
                upper += lower_limit - lower;
                lower = lower_limit;
            } else if (upper > upper_limit) {
                lower += upper_limit - upper;
                upper = upper_limit;
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
