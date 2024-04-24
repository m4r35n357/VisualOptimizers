/*
 * Easom function https://arxiv.org/abs/1008.0549
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real PI; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->PI = acosl(-1.0L);
    return m;
}

minima *get_known_minima () {
    real PI = acosl(-1.0L);
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min->x);
    m->min[0].x[0] = PI + 1.0L;
    m->min[0].x[1] = PI + 2.0L;
    m->min[0].x[2] = PI + 3.0L;
    m->min[0].f = -1.0L;
    return m;
}

void cost (int n, point *p, const model *m) {
    real prod = 1.0L, sum = 0.0L;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - (real)(i + 1);
        prod *= SQR(cosl(xi));
        sum -= SQR(xi - m->PI);
    }
    p->f = - prod * expl(sum);
}
