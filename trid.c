/*
 * Trid function https://www.sfu.ca/~ssurjano/trid.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, *shift; };

model *model_init (int n) {
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
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
        o->min->x[i] = (i + 1) * (n + 1 - (i + 1)) + m->shift[i];
    }
    o->min->f = - n * (n + 4) * (n - 1) / 6.0L;
    return o;
}

void cost (int n, point *p, const model *m) {
    p->f = 0.0L;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - m->shift[i];
        real xi_1 = p->x[i - 1] - m->shift[i - 1];
        p->f += SQR(xi - 1.0L) - (i > 0 ? xi * xi_1 : 0.0L);
    }
}
