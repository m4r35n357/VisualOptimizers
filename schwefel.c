#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real min_edge, max_edge; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->min_edge = -500.0L;  // inequality constraint
    m->max_edge = 500.0L;  // inequality constraint
    return m;
}

minima *get_known_minima () {
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min->x);
    m->min[0].x[0] = 410.9687L;
    m->min[0].x[1] = 400.9687L;
    m->min[0].x[2] = 390.9687L;
    m->min[0].f = 0.0L;
    return m;
}

void cost (int n, point *p, const model *m) { (void)m;
    p->f = 418.9829L * n;
    for (int i = 0; i < n; i++) {
        p->x[i] = p->x[i] >= m->min_edge ? p->x[i] : m->min_edge;
        p->x[i] = p->x[i] <= m->max_edge ? p->x[i] : m->max_edge;
        real xi = p->x[i] + (real)(10 * (i + 1));
        p->f -= xi * sinl(sqrtl(fabsl(xi)));
    }
}
