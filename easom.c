/*
 * Easom function https://arxiv.org/abs/1008.0549
 */
#include <math.h>
#include "model.h"

struct Model { real PI, min_edge, max_edge; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->min_edge = -100.0L;  // inequality constraint
    m->max_edge =  100.0L;  // inequality constraint
    m->PI = acosl(-1.0L);
    return m;
}

minima *get_known_minima (int n) {
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 1;
    o->min = get_point(n); CHECK(o->min);
    for (int i = 0; i < n; i++) {
        o->min->x[i] = acosl(-1.0L);
    }
    o->min->f = -1.0L;
    return o;
}

void cost (int n, point *p, const model *m) {
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= m->min_edge || p->x[i] >= m->max_edge) {
            p->f = INFINITY;
            return;
        }
    }
    real prod = 1.0L, sum = 0.0L;
    for (int i = 0; i < n; i++) {
        prod *= SQR(cosl(p->x[i]));
        sum -= SQR(p->x[i] - m->PI);
    }
    p->f = - prod * expl(sum);
}
