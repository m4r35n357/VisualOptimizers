#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real _; };

model *model_init () {
    return NULL;
}

minima *get_known_minima () {
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min->x);
    m->min[0].x[0] = 4.0L;
    m->min[0].x[1] = 6.0L;
    m->min[0].x[2] = 6.0L;
    m->min[0].f = -7.0L;
    return m;
}

void cost (int n, point *p, const model *m) { (void)m;
    p->f = 0.0L;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - (real)(i + 1);
        real xi_1 = p->x[i - 1] - (real)i;
        p->f += SQR(xi - 1.0L) - ((i > 0) ? xi * xi_1 : 0.0L);
    }
}
