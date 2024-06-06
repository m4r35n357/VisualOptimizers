/*
 * Trid function https://www.sfu.ca/~ssurjano/trid.html
 */
#include <math.h>
#include "model.h"

struct Model { real a, min_edge, max_edge; };

model *model_init (int n) {
    model *m = malloc(sizeof (model));
    m->min_edge = - n * n;  // inequality constraint
    m->max_edge = - m->min_edge;  // inequality constraint
    m->a = 1.0L;
    return m;
}

minima *get_known_minima (int n) {
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 1;
    o->min = get_point(n); CHECK(o->min);
    for (int i = 0; i < n; i++) {
        o->min->x[i] = (i + 1) * (n + 1 - (i + 1));
    }
    o->min->f = - n * (n + 4) * (n - 1) / 6.0L;
    return o;
}

void cost (int n, point *p, const model *m) {
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= m->min_edge || p->x[i] >= m->max_edge) {
            p->f = INFINITY;
            return;
        }
    }
    p->f = 0.0L;
    for (int i = 0; i < n; i++) {
        p->f += SQR(p->x[i] - 1.0L) - (i > 0 ? p->x[i] * p->x[i - 1] : 0.0L);
    }
}
