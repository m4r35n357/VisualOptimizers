
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "nelder_mead.h"

/*
 * Initial point at centroid, all vertices equally spaced, trial points allocated
 */
simplex *get_simplex (int n, real size, const point *start) {
    simplex *s = malloc(sizeof (simplex));              CHECK(s);
    s->n = n;
    s->p = malloc((size_t)(n + 1) * sizeof (point));    CHECK(s->p);
    for (int i = 0; i < n + 1; i++) {  // simplex vertices
        s->p[i] = *get_point(n);
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
    s->expand = get_point(n);
    s->contract = get_point(n);
    s->centre = get_point(n);
    s->iterations = s->evaluations = 0;
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
    real ALPHA = 1.0L;
    real GAMMA = o->adaptive ? 1.0L + 2.0L / s->n : 2.0L;
    real RHO = o->adaptive ? 0.75L - 0.5L / s->n : 0.5L;
    real SIGMA = o->adaptive ? 1.0L - 1.0L / s->n : 0.5L;
    point *best = s->p;
    point *worst = s->p + s->n;
    point *second_worst = worst - 1;
    for (int i = 0; i < s->n + 1; i++) {  // initial cost at simplex vertices
        cost(s->n, s->p + i, m);
        s->evaluations++;
    }
    sort(s);
    printf(o->fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter %s% .*Lf\n",
           GRY, NRM, o->precision, distance(s->n, best, worst));
    while (distance(s->n, best, worst) > o->x_tolerance || (worst->f - best->f) > o->f_tolerance) {
        CHECK(s->evaluations <= o->max_evaluations);
        CHECK(s->iterations <= o->max_iterations);
        if (s->looping) goto resume; else s->looping = true;
        int shrink = 0;
        get_centroid(s, s->centre);
        project(s->reflect, s, m, s->centre, ALPHA, worst, s->centre);
        if (s->reflect->f < best->f) {
            project(s->expand, s, m, s->centre, GAMMA, worst, s->centre);
            if (s->expand->f < s->reflect->f) {
                if (o->debug) printf("expand        ");
                copy_point(s->n, s->expand, worst);
            } else {
                if (o->debug) printf("reflect       ");
                copy_point(s->n, s->reflect, worst);
            }
        } else {
            if (s->reflect->f < second_worst->f) {
                if (o->debug) printf("reflect       ");
                copy_point(s->n, s->reflect, worst);
            } else {
                if (s->reflect->f < worst->f) {
                    project(s->contract, s, m, s->centre, RHO, worst, s->centre);
                    if (s->contract->f < s->reflect->f) {
                        if (o->debug) printf("contract_out  ");
                        copy_point(s->n, s->contract, worst);
                    } else {
                        shrink = 1;
                    }
                } else {
                    project(s->contract, s, m, s->centre, RHO, s->centre, worst);
                    if (s->contract->f <= worst->f) {
                        if (o->debug) printf("contract_in   ");
                        copy_point(s->n, s->contract, worst);
                    } else {
                        shrink = 1;
                    }
                }
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
            for (int i = 0; i < s->n; i++) {
                printf(o->fmt ? "% .*Le " : "% .*Lf ", o->precision, best->x[i]);
            }
            printf(o->fmt ? "]  % .*Le\n" : "]  % .*Lf\n", o->precision, best->f);
        }
        return true;
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
}

/*
 * Get centroid (average position) of simplex
 */
void get_centroid (const simplex *s, point *c) {
    for (int j = 0; j < s->n; j++) {
        c->x[j] = 0.0L;
        for (int i = 0; i < s->n; i++) {
            c->x[j] += s->p[i].x[j];
        }
        c->x[j] /= s->n;
    }
}

/*
 * Take the line from pa to pb, shift it to start, and scale it's length by factor
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

void print_point (int n, const point *p, int dp, int fmt) {
    printf("[ %s", NRM);
    for (int i = 0; i < n; i++) {
        printf(fmt ? "% .*Le " : "% .*Lf ", dp, p->x[i]);
    }
    printf(fmt ? "%s]%s % .*Le\n" : "%s]%s % .*Lf\n", GRY, NRM, dp, p->f);
}
