/*
 * Schwefel function https://www.sfu.ca/~ssurjano/schwef.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real min_edge, max_edge; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->min_edge = -500.0L;  // inequality constraint
    m->max_edge =  500.0L;  // inequality constraint
    return m;
}

minima *get_known_minima (int n) {
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 1;
    o->min = get_point(n); CHECK(o->min);
    for (int i = 0; i < n; i++) {
        o->min->x[i] = 420.9687L;
    }
    o->min->f = 0.0L;
    return o;
}

void cost (int n, point *p, const model *m) {
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= m->min_edge || p->x[i] >= m->max_edge) {
            p->f = INFINITY;
            return;
        }
    }
    p->f = 418.9829L * n;
    for (int i = 0; i < n; i++) {
        p->f -= p->x[i] * sinl(sqrtl(fabsl(p->x[i])));
    }
}
