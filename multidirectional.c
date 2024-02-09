#include <stdio.h>
#include <stdlib.h>
#include "simplex.h"

#define ALPHA 1.0L
#define GAMMA 2.0L
#define RHO 0.5L
#define SIGMA 0.5L

optimset get_settings (char **argv, bool single) {
    optimset opt = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .tolerance = strtold(argv[3], NULL),
        .max_iterations = (int)strtol(argv[4], NULL, BASE),
        .size = strtold(argv[5], NULL),
        .step_mode = single
    };
    CHECK(opt.places >= 3 && opt.places <= 36);
    CHECK(opt.fmt == 0 || opt.fmt == 1);
    CHECK(opt.tolerance >= 1.0e-36L && opt.tolerance <= 1.0e-3L);
    CHECK(opt.max_iterations >= 1 && opt.max_iterations <= 100000);
    CHECK(opt.size >= 1.0e-12L && opt.size <= 1.0e3L);
    return opt;
}

/*
 * Initialize for Multidirectional Search
 */
simplex *get_mds_simplex (int n, real size, const point *start) {
    simplex *s = get_regular_simplex (n, size, start);
    s->reflect = malloc((size_t)(n + 1) * sizeof (point));   CHECK(s->reflect);
    for (int i = 0; i < n + 1; i++) {
        s->reflect[i].x = malloc((size_t)n * sizeof (real)); CHECK(s->reflect[i].x);
    }
    s->trial = malloc((size_t)(n + 1) * sizeof (point));     CHECK(s->trial);
    for (int i = 0; i < n + 1; i++) {
        s->trial[i].x = malloc((size_t)n * sizeof (real));   CHECK(s->trial[i].x);
    }
    s->centroid = get_point(n);
    s->iterations = s->evaluations = 0;
    s->looping = false;
    return s;
}

/*
 * Multidirectional Search Optimizer
 */
bool multidirectional_search (simplex *s, const model *m, const optimset *o) {
    point *best = s->p;
    point *worst = s->p + s->n;
    if (o->step_mode && s->looping) goto resume; else s->looping = true;
    while (s->delta_x > o->tolerance || s->delta_f > o->tolerance) {
        CHECK(s->iterations <= o->max_iterations);
        multi_project(s->reflect, s, m, ALPHA);
        if (s->reflect[0].f < best->f) {
            multi_project(s->trial, s, m, GAMMA);
            if (s->trial[0].f < s->reflect[0].f) {
                printf("expand        ");
                for (int i = 0; i < s->n + 1; i++) {
                    copy_point(s->n, s->trial + i, s->p + i);
                }
            } else {
                printf("reflect       ");
                for (int i = 0; i < s->n + 1; i++) {
                    copy_point(s->n, s->reflect + i, s->p + i);
                }
            }
        } else {
            printf("contract      ");
            for (int i = 0; i < s->n + 1; i++) {
                point *non_best = s->p + i;
                project(non_best, s, m, - SIGMA, non_best, best);
            }
            qsort((void *)(s->p), (size_t)s->n + 1, sizeof (point), compare);
        }
        s->delta_x = distance(s->n, worst, best);
        s->delta_f = worst->f - best->f;
        s->iterations++;
        printf(" %04d %04d  [ ", s->iterations, s->evaluations);
        for (int j = 0; j < s->n; j++) {
            printf(o->fmt ? "% .*Le " : "% .*Lf ", o->places, best->x[j]);
        }
        printf(o->fmt ? "]  % .*Le  % .*Le % .*Le\n" : "]  % .*Lf  % .*Lf % .*Lf\n",
                o->places, best->f, o->places, s->delta_x, o->places, s->delta_f);
        if (o->step_mode) return true;
        resume: ;
    }
    return s->looping = false;
}

void multi_project (point *new, simplex *s, const model *m, real factor) {
    for (int i = 0; i < s->n + 1; i++) {
        project(new + i, s, m, factor, s->p + i, s->p);
    }
    qsort((void *)(new), (size_t)s->n + 1, sizeof (point), compare);
}
