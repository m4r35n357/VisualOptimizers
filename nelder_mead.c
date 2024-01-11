
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "nelder_mead.h"

optimset get_settings (char **argv) {
    optimset opt = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .debug = (int)strtol(argv[3], NULL, BASE),
        .tolerance = strtold(argv[4], NULL),
        .max_iterations = (int)strtol(argv[5], NULL, BASE),
        .size = strtold(argv[6], NULL)
    };
    CHECK(opt.places >= 3 && opt.places <= 36);
    CHECK(opt.debug == 0 || opt.debug == 1);
    CHECK(opt.tolerance >= 1.0e-36L && opt.tolerance <= 1.0e-3L);
    CHECK(opt.max_iterations >= 1 && opt.max_iterations <= 100000);
    CHECK(opt.size >= 1.0e-12L && opt.size <= 1.0e3L);
    return opt;
}

/*
 * Initial point at centroid, all vertices equally spaced, trial points allocated
 */
simplex *get_simplex (int n, real size, const point *start, const model *m) {
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
    s->reflect = get_point(n);
    s->centroid = get_point(n);
    s->trial = get_point(n);
    s->iterations = s->evaluations = 0;
    s->gl = s->looping = false;
    for (int i = 0; i < s->n + 1; i++) {  // initial cost at simplex vertices
        cost(s->n, s->p + i, m);
        s->evaluations++;
    }
    sort(s);
    return s;
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
 * Nelder-Mead Optimizer
 */
bool nelder_mead (simplex *s, point *solution, const model *m, const optimset *o) {
    point *best = s->p;
    point *worst = s->p + s->n;
    point *second_worst = worst - 1;
    if (s->gl && s->looping) goto resume; else s->looping = true;
    printf(o->fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter %s% .*Lf\n",
           GRY, NRM, o->places, distance(s->n, best, worst));
    while (s->delta_x > o->tolerance || s->delta_f > o->tolerance) {
        CHECK(s->iterations <= o->max_iterations);
        int shrink = 0;
        project(s->reflect, s, m, s->centroid, ALPHA, worst, s->centroid);
        if (best->f <= s->reflect->f && s->reflect->f < second_worst->f) {
            if (o->debug) printf("reflect       ");
            copy_point(s->n, s->reflect, worst);
        } else if (s->reflect->f < best->f) {
            project(s->trial, s, m, s->centroid, GAMMA, worst, s->centroid);
            if (s->trial->f < s->reflect->f) {
                if (o->debug) printf("expand        ");
                copy_point(s->n, s->trial, worst);
            } else {
                if (o->debug) printf("reflect       ");
                copy_point(s->n, s->reflect, worst);
            }
        } else if (s->reflect->f < worst->f) {
            project(s->trial, s, m, s->centroid, RHO, worst, s->centroid);
            if (s->trial->f < s->reflect->f) {
                if (o->debug) printf("contract_out  ");
                copy_point(s->n, s->trial, worst);
            } else {
                shrink = 1;
            }
        } else {
            project(s->trial, s, m, s->centroid, RHO, s->centroid, worst);
            if (s->trial->f < worst->f) {
                if (o->debug) printf("contract_in   ");
                copy_point(s->n, s->trial, worst);
            } else {
                shrink = 1;
            }
        }
        if (shrink) {
            if (o->debug) printf("shrink        ");
            for (int i = 1; i < s->n + 1; i++) {
                point *non_best = s->p + i;
                project(non_best, s, m, non_best, SIGMA, non_best, best);
            }
        }
        sort(s);
        s->iterations++;
        if (o->debug) { // print current minimum
            printf(" %04d %04d  [ ", s->iterations, s->evaluations);
            for (int j = 0; j < s->n; j++) {
                printf(o->fmt ? "% .*Le " : "% .*Lf ", o->places, best->x[j]);
            }
            printf(o->fmt ? "]  % .*Le  % .*Le % .*Le\n" : "]  % .*Lf  % .*Lf % .*Lf\n",
                    o->places, best->f, o->places, s->delta_x, o->places, s->delta_f);
        }
        if (s->gl) return true;
        resume: ;
    }
    // save solution in output argument
    copy_point(s->n, best, solution);
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
    s->delta_f = s->p[0].f - s->p[s->n].f;
}

/*
 * Take the line from pa to pb, shift it to start, and scale its length by factor
 */
void project (point *new, simplex *s, const model *m, const point *start, real factor, const point *pa, point *pb) {
    for (int j = 0; j < s->n; j++) {
        new->x[j] = start->x[j] + factor * (pb->x[j] - pa->x[j]);
    }
    cost(s->n, new, m);
    s->evaluations++;
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
    printf("[ %s", NRM);
    for (int i = 0; i < n; i++) {
        printf(fmt ? "% .*Le " : "% .*Lf ", places, p->x[i]);
    }
    printf(fmt ? "%s]%s % .*Le\n" : "%s]%s % .*Lf\n", GRY, NRM, places, p->f);
}
