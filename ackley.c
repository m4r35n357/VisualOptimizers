/*
 * Ackley function https://www.sfu.ca/~ssurjano/ackley.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b, c; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->a = 20.0L;
    m->b = 0.2L;
    m->c = 2.0L * acosl(-1.0L);
    return m;
}

minima *get_known_minima () {
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min->x);
    m->min[0].x[0] = 0.0L;
    m->min[0].x[1] = 1.0L;
    m->min[0].x[2] = 2.0L;
    m->min[0].f = 0.0L;
    return m;
}

void cost (int n, point *p, const model *m) {
    real sum_sqr = 0.0L;
    real sum_cos = 0.0L;
    for (int i = 0; i < n; i++) {
        sum_sqr += SQR(p->x[i] - (real)i);
        sum_cos += cosl(m->c * (p->x[i] - (real)i));
    }
    p->f = -m->a * expl(-m->b * sqrtl(sum_sqr / n)) - expl(sum_cos / n) + m->a + expl(1.0L);
}
