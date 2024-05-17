/*
 * Rastrigin function https://www.sfu.ca/~ssurjano/rastr.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real A, TWO_PI, *shift; };

model *model_init (int n) {
    model *m = malloc(sizeof (model));
    m->A = 10.0L;
    m->TWO_PI = 2.0L * acosl(-1.0L);
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
        o->min->x[i] = m->shift[i];
    }
    o->min->f = 0.0L;
    return o;
}

void cost (int n, point *p, const model *m) {
    p->f = m->A * n;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - m->shift[i];
        p->f += SQR(xi) - m->A * cosl(m->TWO_PI * xi);
    }
}
