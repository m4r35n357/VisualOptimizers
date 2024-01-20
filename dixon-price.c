/*
 * Dixon-Price function https://www.sfu.ca/~ssurjano/dixonpr.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
    m->b = 2.0L;
    return m;
}

minima *get_known_minima () {
    real y = 0.5L * sqrtl(2.0L);
    real z = powl(2.0L, -0.75L);
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 4;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min[0].x);
    m->min[0].x[0] = 1.0L;
    m->min[0].x[1] = y;
    m->min[0].x[2] = z;
    m->min[0].f = 0.0L;
    m->min[1].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min[1].x);
    m->min[1].x[0] = 1.0L;
    m->min[1].x[1] = -y;
    m->min[1].x[2] = z;
    m->min[1].f = 0.0L;
    m->min[2].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min[2].x);
    m->min[2].x[0] = 1.0L;
    m->min[2].x[1] = y;
    m->min[2].x[2] = -z;
    m->min[2].f = 0.0L;
    m->min[3].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min[3].x);
    m->min[3].x[0] = 1.0L;
    m->min[3].x[1] = -y;
    m->min[3].x[2] = -z;
    m->min[3].f = 0.0L;
    return m;
}

void cost (int n, point *p, const model *m) {
    p->f = SQR(p->x[0] - m->a);
    for (int i = 1; i < n; i++) {
        p->f += (i + 1) * SQR(m->b * SQR(p->x[i]) - p->x[i - 1]);
    }
}
