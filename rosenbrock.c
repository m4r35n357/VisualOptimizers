/*
 * Rosenbrock function https://www.sfu.ca/~ssurjano/rosen.html
 */
#include <stdio.h>
#include <stdlib.h>
#include "model.h"

struct Model { real a, b; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
    m->b = 100.0L;
    return m;
}

minima *get_known_minima () {
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min[0].x);
    m->min[0].x[0] = 1.0L;
    m->min[0].x[1] = 2.0L;
    m->min[0].x[2] = 3.0L;
    m->min[0].f = 0.0L;
    return m;
}

void cost (int n, point *p, const model *m) {
    p->f = 0.0L;
    for (int i = 0; i < n - 1; i++) {
        real xi = p->x[i] - (real)(i);
        real xi_1 = p->x[i + 1] - (real)(i + 1);
        p->f += SQR(m->a - xi) + m->b * SQR(xi_1 - SQR(xi));
    }
}
