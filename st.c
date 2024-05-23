/*
 * Styblinski-Tang function https://www.sfu.ca/~ssurjano/stybtang.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b, min_edge, max_edge; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->min_edge = -5.0L;  // inequality constraint
    m->max_edge =  5.0L;  // inequality constraint
    m->a = 16.0L;
    m->b = 5.0L;
    return m;
}

minima *get_known_minima (int n) {
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 1;
    o->min = get_point(n); CHECK(o->min);
    o->min->x[0] = -1.903534L;
    o->min->x[1] = -0.903534L;
    o->min->x[2] = 0.096466L;
    o->min->f = -117.498497L;
    return o;
}

void cost (int n, point *p, const model *m) {
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= m->min_edge || p->x[i] >= m->max_edge) {
            p->f = INFINITY;
            return;
        }
    }
    real sum = 0.0L;
    for (int i = 0; i < n; i++) {
        real sqr = SQR(p->x[i]);
        sum += SQR(sqr) - m->a * sqr + m->b * p->x[i];
    }
    p->f = 0.5L * sum;
}
