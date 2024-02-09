#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
 * Initial point at centre, all vertices equally spaced, trial points allocated
 */
simplex *get_mds_simplex (int n, real size, const point *start) {
    simplex *s = malloc(sizeof (simplex));              CHECK(s);
    s->n = n;
    s->p = malloc((size_t)(n + 1) * sizeof (point));    CHECK(s->p);
    for (int i = 0; i < n + 1; i++) {  // simplex vertices
        s->p[i].x = malloc((size_t)n * sizeof (real));    CHECK(s->p[i].x);
        for (int j = 0; j < n; j++) {  // coordinates
            s->p[i].x[j] = 0.0L;
        }
    }
    real b = 0.0L;
    for (int j = 0; j < n; j++) {
        real c = sqrtl(1.0L - b);
        s->p[j].x[j] = c;
        real r = - (1.0L / n + b) / c;
        for (int i = j + 1; i < n + 1; i++) {
            s->p[i].x[j] = r;
        }
        b += SQR(r);
    }
    for (int i = 0; i < n + 1; i++) {
        for (int j = 0; j < n; j++) {
            s->p[i].x[j] = size * s->p[i].x[j] + start->x[j];
        }
    }
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

/*
 * Euclidean distance between two points
 */
real distance (int n, const point *a, const point *b) {
    real sum = 0.0L;
    for (int j = 0; j < n; j++) {
        sum += SQR(a->x[j] - b->x[j]);
    }
    return sqrtl(sum);
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

void multi_project (point *new, simplex *s, const model *m, real factor) {
    for (int i = 0; i < s->n + 1; i++) {
        project(new + i, s, m, factor, s->p + i, s->p);
    }
    qsort((void *)(new), (size_t)s->n + 1, sizeof (point), compare);
}

/*
 * Point utilities
 */
point *get_point (int n) {
    point *p = malloc(sizeof (point));        CHECK(p);
    p->x = malloc((size_t)n * sizeof (real)); CHECK(p->x);
    return p;
}

void copy_point (int n, const point *src, point *dst) {
    for (int i = 0; i < n; i++) {
        dst->x[i] = src->x[i];
    }
    dst->f = src->f;
}

void print_point (int n, const point *p, int places, int fmt) {
    fprintf(stderr, "%s[%s ", GRY, NRM);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, fmt ? "% .*Le " : "% .*Lf ", places, p->x[i]);
    }
    fprintf(stderr, fmt ? "%s]%s % .*Le\n" : "%s]%s % .*Lf\n", GRY, NRM, places, p->f);
}
