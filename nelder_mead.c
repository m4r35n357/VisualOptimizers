#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "simplex.h"

optimset get_settings (char **argv) {
    randomize();
    optimset opt = {
        .step_mode = strstr(argv[0], "-gl") ? true : false,
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .tolerance = strtold(argv[4], NULL),
        .max_evaluations = (int)strtol(argv[5], NULL, BASE),
        .size = strtold(argv[6], NULL),
        .adaptive = (int)strtol(argv[7], NULL, BASE),
        .init_mode = (int)strtol(argv[8], NULL, BASE)
    };
    CHECK(opt.places >= 1 && opt.places <= 36);
    CHECK(opt.fmt == 0 || opt.fmt == 1);
    CHECK(opt.n >= 1 && opt.n <= 64);
    CHECK(opt.tolerance >= 1.0e-36L && opt.tolerance <= 1.0e-3L);
    CHECK(opt.max_evaluations >= 1 && opt.max_evaluations <= 10000000);
    CHECK(opt.size >= 1.0e-12L && opt.size <= 1.0e3L);
    CHECK(opt.adaptive == 0 || opt.adaptive == 1);
    CHECK(opt.init_mode >= 0 && opt.init_mode <= 2);
    return opt;
}

/*
 * Initial point at start, all vertices equally spaced
 */
void regular_simplex (simplex *s, real size, const point *start) {
    real b = 0.0L;
    for (int j = 0; j < s->n; j++) {
        real c = sqrtl(1.0L - b);
        s->p[j].x[j] = c;
        real r = - (1.0L / s->n + b) / c;
        for (int i = j + 1; i < s->n + 1; i++) {
            s->p[i].x[j] = r;
        }
        b += SQR(r);
    }
    for (int i = 0; i < s->n + 1; i++) {
        for (int j = 0; j < s->n; j++) {
            s->p[i].x[j] = size * s->p[i].x[j] + start->x[j];
        }
    }
    s->iterations = s->evaluations = 0;
    s->looping = false;
}

/*
 * Initialize for Nelder-Mead
 */
simplex *nm_simplex (int n, real size, const point *start, bool adaptive) {
    simplex *s = malloc(sizeof (simplex));              CHECK(s);
    s->n = n;
    s->p = malloc((size_t)(n + 1) * sizeof (point));    CHECK(s->p);
    for (int i = 0; i < n + 1; i++) {  // simplex vertices
        s->p[i].x = malloc((size_t)n * sizeof (real));    CHECK(s->p[i].x);
        for (int j = 0; j < n; j++) {  // coordinates
            s->p[i].x[j] = 0.0L;
        }
    }
    s->ALPHA = 1.0L;
    s->GAMMA = adaptive ? 1.0L + 2.0L / n : 2.0L;
    s->RHO = adaptive ? 0.75L - 0.5L / n : 0.5L;
    s->SIGMA = adaptive ? 1.0L - 1.0L / n : 0.5L;
    s->reflect = get_point(n);
    s->centroid = get_point(n);
    s->trial = get_point(n);
    regular_simplex(s, size, start);
    return s;
}

/*
 * Nelder-Mead Optimizer
 */
bool nelder_mead (simplex *s, const model *m, const optimset *o) {
    point *best = s->p, *worst = s->p + s->n, *second_worst = worst - 1;
    if (o->step_mode && s->looping) goto resume; else s->looping = true;
    while ((s->delta_x > o->tolerance || s->delta_f > o->tolerance) && s->evaluations <= o->max_evaluations) {
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
        print_progress(s, best, o->places, o->fmt);
        if (o->step_mode) return true;
        resume: ;
    }
    return s->looping = false;
}

/*
 * Simplex sorting
 */
int compare (const void *arg1, const void *arg2) {
    const real f1 = ((const point *)arg1)->f;
    const real f2 = ((const point *)arg2)->f;
    return (f1 > f2) - (f1 < f2);
}

void sort (simplex *s) {
    qsort((void *)(s->p), (size_t)s->n + 1, sizeof (point), compare);
    for (int j = 0; j < s->n; j++) {
        s->centroid->x[j] = 0.0L;
        for (int i = 0; i < s->n; i++) {
            s->centroid->x[j] += s->p[i].x[j];
        }
        s->centroid->x[j] /= s->n;
    }
    s->delta_x = distance(s->n, s->p, s->p + s->n);
    s->delta_f = s->p[s->n].f - s->p[0].f;
}

/*
 * Take the line from pa to pb, shift it to pb, and scale its length by factor
 */
void project (point *new, simplex *s, const model *m, real factor, const point *pa, const point *pb) {
    for (int j = 0; j < s->n; j++) {
        new->x[j] = pb->x[j] + factor * (pb->x[j] - pa->x[j]);
    }
    cost(s->n, new, m);
    s->evaluations++;
}

/*
 * Progress output
 */
void print_progress (const simplex *s, const point *best, int places, int fmt) {
    fprintf(stdout, " %4d %4d  [ ", s->iterations, s->evaluations);
    for (int j = 0; j < s->n; j++) {
        fprintf(stdout, fmt ? "% .*Le " : "% .*Lf ", places, best->x[j]);
    }
    fprintf(stdout, fmt ? "]  % .*Le  % .*Le % .*Le\n" : "]  % .*Lf  ( % .*Lf % .*Lf )\n",
            places, best->f, places, s->delta_x, places, s->delta_f);
}
