/*
 * Easom function https://arxiv.org/abs/1008.0549
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real PI, *shift; };

model *model_init (int n) {
    model *m = malloc(sizeof (model));
    m->PI = acosl(-1.0L);
    m->shift = malloc((size_t)n * sizeof (real)); CHECK(m->shift);
    for (int i = 0; i < n; i++) {
        m->shift[i] = (real)(i + 1);
    }
    return m;
}

minima *get_known_minima (int n, const model *m) {
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 1;
    o->min = get_point(n); CHECK(o->min);
    for (int i = 0; i < n; i++) {
        o->min->x[i] = m->PI + m->shift[i];
    }
    o->min->f = -1.0L;
    return o;
}

void cost (int n, point *p, const model *m) {
    real prod = 1.0L, sum = 0.0L;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - m->shift[i];
        prod *= SQR(cosl(xi));
        sum -= SQR(xi - m->PI);
    }
    p->f = - prod * expl(sum);
}
