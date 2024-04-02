/*
 * "Treacle" function - of my own devising. Very challenging for simplex-based methods (also non-convex)
 */
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
    m->min[0].x[0] = 1.0L;
    m->min[0].x[1] = 2.0L;
    m->min[0].x[2] = 3.0L;
    m->min[0].f = 0.0L;
    return m;
}

void cost (int n, point *p, const model *m) { (void)m;
    p->f = 0.0L;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - (real)(i + 1);
        p->f += sqrtl(fabsl(xi));
    }
}
