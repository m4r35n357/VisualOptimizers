#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simplex.h"

/*
 * Initial point at centre, all vertices equally spaced, trial points allocated
 */
simplex *get_regular_simplex (int n, real size, const point *start) {
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
