#include <stdio.h>
#include <stdlib.h>
#include "simplex.h"

optimset get_settings (char **argv, bool single) {
    optimset opt = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .tolerance = strtold(argv[4], NULL),
        .max_iterations = (int)strtol(argv[5], NULL, BASE),
        .adaptive = (int)strtol(argv[6], NULL, BASE),
        .lower = strtold(argv[7], NULL),
        .upper = strtold(argv[8], NULL),
        .step_mode = single
    };
    CHECK(opt.places >= 3 && opt.places <= 36);
    CHECK(opt.fmt == 0 || opt.fmt == 1);
    CHECK(opt.n >= 1);
    CHECK(opt.tolerance >= 1.0e-36L && opt.tolerance <= 1.0e-3L);
    CHECK(opt.max_iterations >= 1 && opt.max_iterations <= 100000);
    CHECK(opt.adaptive == 0 || opt.adaptive == 1);
    return opt;
}

/*
 * Initialize for Nelder-Mead
 */
simplex *nm_simplex (int n, real size, const point *start, bool adaptive) {
    simplex *s = regular_simplex(n, size, start);
    s->ALPHA = 1.0L;
    s->GAMMA = adaptive ? 1.0L + 2.0L / n : 2.0L;
    s->RHO = adaptive ? 0.75L - 0.5L / n : 0.5L;
    s->SIGMA = adaptive ? 1.0L - 1.0L / n : 0.5L;
    s->reflect = get_point(n);
    s->centroid = get_point(n);
    s->trial = get_point(n);
    s->iterations = s->evaluations = 0;
    s->looping = false;
    return s;
}

/*
 * Nelder-Mead Optimizer
 */
bool nelder_mead (simplex *s, const model *m, const optimset *o) {
    point *best = s->p;
    point *worst = s->p + s->n;
    point *second_worst = worst - 1;
    if (o->step_mode && s->looping) goto resume; else s->looping = true;
    while (s->delta_x > o->tolerance || s->delta_f > o->tolerance) {
        CHECK(s->iterations <= o->max_iterations);
        int shrink = 0;
        project(s->reflect, s, m, s->ALPHA, worst, s->centroid);
        if (best->f <= s->reflect->f && s->reflect->f < second_worst->f) {
            printf("reflect       ");
            copy_point(s->n, s->reflect, worst);
        } else if (s->reflect->f < best->f) {
            project(s->trial, s, m, s->GAMMA, worst, s->centroid);
            if (s->trial->f < s->reflect->f) {
                printf("expand        ");
                copy_point(s->n, s->trial, worst);
            } else {
                printf("reflect       ");
                copy_point(s->n, s->reflect, worst);
            }
        } else if (s->reflect->f < worst->f) {
            project(s->trial, s, m, s->RHO, worst, s->centroid);
            if (s->trial->f < s->reflect->f) {
                printf("contract_out  ");
                copy_point(s->n, s->trial, worst);
            } else shrink = 1;
        } else {
            project(s->trial, s, m, - s->RHO, worst, s->centroid);
            if (s->trial->f < worst->f) {
                printf("contract_in   ");
                copy_point(s->n, s->trial, worst);
            } else shrink = 1;
        }
        if (shrink) {
            printf("shrink        ");
            for (int i = 1; i < s->n + 1; i++) {
                point *non_best = s->p + i;
                project(non_best, s, m, - s->SIGMA, non_best, best);
            }
        }
        sort(s);
        s->iterations++;
        print_progress(s, best, o);
        if (o->step_mode) return true;
        resume: ;
    }
    return s->looping = false;
}
