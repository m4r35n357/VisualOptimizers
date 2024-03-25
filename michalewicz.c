/*
 * Michalewicz function https://www.sfu.ca/~ssurjano/michal.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real m, PI, min_edge, max_edge; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->m = 1.0L;
    m->PI = acosl(-1.0L);
    m->min_edge = 0.0L;  // inequality constraint
    m->max_edge = m->PI;  // inequality constraint
    return m;
}

minima *get_known_minima () {
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min[0].x);
    m->min[0].x[0] = 2.071689L;
    m->min[0].x[1] = 1.570796L;
    m->min[0].x[2] = 1.304668L;
    m->min[0].f = -2.802848L;
    return m;
}

void cost (int n, point *p, const model *m) { (void)m;
    p->f =0.0L;
    for (int i = 0; i < n; i++) {
        p->x[i] = p->x[i] >= m->min_edge ? p->x[i] : m->min_edge;
        p->x[i] = p->x[i] <= m->max_edge ? p->x[i] : m->max_edge;
        p->f -= sinl(p->x[i]) * powl(sinl((i + 1) * SQR(p->x[i]) / m->PI), 2.0L * m->m);
    }
}
