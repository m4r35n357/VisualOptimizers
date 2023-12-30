
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nelder_mead.h"

/*
 * Initial point at centroid, all vertices equally spaced
 */
simplex *regular (int n, real size, const point *centre) {
    simplex *s = malloc(sizeof (simplex));
    s->n = n;
    s->p = malloc((size_t)(n + 1) * sizeof (point));
    for (int i = 0; i < n + 1; i++) {  // simplex vertices
        s->p[i].x = malloc((size_t)n * sizeof (real));
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
            s->p[i].x[j] = size * s->p[i].x[j] + centre->x[j];
        }
    }
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
 * Main function
 * - start is the initial point (unchanged in output)
 * - n is the dimension of the data
 * - solution is the minimizer
 * - args are the optional arguments of cost_function
 * - opt are the optimisation settings
 */
simplex *nelder_mead (int n, const point *start, point *solution, const parameters *m, const optimset *o) {
    real ALPHA = 1.0L;
    real GAMMA = o->adaptive_scaling ? 1.0L + 2.0L / n : 2.0L;
    real RHO = o->adaptive_scaling ? 0.75L - 0.5L / n : 0.5L;
    real SIGMA = o->adaptive_scaling ? 1.0L - 1.0L / n : 0.5L;

    // labels
    point *best, *worst, *second_worst, *non_best;

    // initial simplex has size n + 1 where n is the dimensionality of the data
    simplex *s = regular(n, o->simplex_scaling, start);
    // allocate memory for internal points
    s->reflected = malloc(sizeof (point));  CHECK(s->reflected);
    s->expanded = malloc(sizeof (point));   CHECK(s->expanded);
    s->contracted = malloc(sizeof (point)); CHECK(s->contracted);
    s->centre = malloc(sizeof (point));     CHECK(s->centre);
    s->reflected->x = malloc((size_t)n * sizeof (real));  CHECK(s->reflected->x);
    s->expanded->x = malloc((size_t)n * sizeof (real));   CHECK(s->expanded->x);
    s->contracted->x = malloc((size_t)n * sizeof (real)); CHECK(s->contracted->x);
    s->centre->x = malloc((size_t)n * sizeof (real));     CHECK(s->centre->x);
    s->iterations = 0;
    s->evaluations = 0;
    for (int i = 0; i < n + 1; i++) {  // simplex vertices
        cost(n, s->p + i, m);
        s->evaluations++;
    }
    sort(s);
    best = s->p;
    worst = s->p + n;
    second_worst = worst - 1;
    printf(o->fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter %s% .*Lf\n",
            GRY, NRM, o->diplay_precision, distance(n, best, worst));

    while (processing(s, o)) {
        int shrink = 0;
        get_centroid(s, s->centre);

        project(s->reflected, n, s->centre, ALPHA, worst, s->centre);
        cost(n, s->reflected, m);
        s->evaluations++;
        if (s->reflected->f < best->f) {
            project(s->expanded, n, s->centre, GAMMA, worst, s->centre);
            cost(n, s->expanded, m);
            s->evaluations++;
            if (s->expanded->f < s->reflected->f) {
                if (o->verbose) printf("expand        ");
                copy_point(n, s->expanded, worst);
            } else {
                if (o->verbose) printf("reflect       ");
                copy_point(n, s->reflected, worst);
            }
        } else {
            if (s->reflected->f < second_worst->f) {
                if (o->verbose) printf("reflect       ");
                copy_point(n, s->reflected, worst);
            } else {
                if (s->reflected->f < worst->f) {
                    project(s->contracted, n, s->centre, RHO, worst, s->centre);
                    cost(n, s->contracted, m);
                    s->evaluations++;
                    if (s->contracted->f < s->reflected->f) {
                        if (o->verbose) printf("contract_out  ");
                        copy_point(n, s->contracted, worst);
                    } else {
                        shrink = 1;
                    }
                } else {
                    project(s->contracted, n, s->centre, RHO, s->centre, worst);
                    cost(n, s->contracted, m);
                    s->evaluations++;
                    if (s->contracted->f <= worst->f) {
                        if (o->verbose) printf("contract_in   ");
                        copy_point(n, s->contracted, worst);
                    } else {
                        shrink = 1;
                    }
                }
            }
        }
        if (shrink) {
            if (o->verbose) printf("shrink        ");
            for (int i = 1; i < n + 1; i++) {
            	non_best = s->p + i;
                project(non_best, n, non_best, SIGMA, non_best, best);
                cost(n, s->p + i, m);
                s->evaluations++;
            }
        }
        sort(s);
    	s->iterations++;

        if (o->verbose) { // print current minimum
            printf(" %04d %04d  [ ", s->iterations, s->evaluations);
            for (int i = 0; i < n; i++) {
                printf(o->fmt ? "% .*Le " : "% .*Lf ", o->diplay_precision, best->x[i]);
            }
            printf(o->fmt ? "]  % .*Le\n" : "]  % .*Lf\n", o->diplay_precision, best->f);
        }
    }

    // save solution in output argument
    copy_point(n, best, solution);

    return s;
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
    qsort((void *)(s->p), (size_t)s->n + 1, sizeof(point), compare);
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
 * Terminate or continue?
 */
int processing (const simplex *s, const optimset *opt) {
    CHECK(s->evaluations <= opt->max_eval);
    CHECK(s->iterations <= opt->max_iter);
    const int n = s->n;
    return distance(n, s->p, s->p + n) > opt->tolx || (s->p[n].f - s->p[0].f) > opt->tolf;
}

/*
 * Project from a starting point along a line (pb - pa), scaled by factor
 */
void project (const point *new, int n, const point *start, real factor, const point *pa, point *pb) {
    for (int j = 0; j < n; j++) {
        new->x[j] = start->x[j] + factor * (pb->x[j] - pa->x[j]);
    }
}

void copy_point (int n, const point *src, point *dst) {
    memcpy(dst->x, src->x, sizeof(real) * (size_t)n);
    dst->f = src->f;
}

void print_point (int n, const point *p, int dp, int fmt) {
    printf("[ %s", NRM);
    for (int i = 0; i < n; i++) {
        printf(fmt ? "% .*Le " : "% .*Lf ", dp, p->x[i]);
    }
    printf(fmt ? "%s]%s % .*Le\n" : "%s]%s % .*Lf\n", GRY, NRM, dp, p->f);
}
