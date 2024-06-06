/*
 * Ackley function https://www.sfu.ca/~ssurjano/ackley.html
 */
#include <math.h>
#include "model.h"

struct Model { real a, b, c, min_edge, max_edge; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->min_edge = -32.768L;  // inequality constraint
    m->max_edge =  32.768L;  // inequality constraint
    m->a = 20.0L;
    m->b = 0.2L;
    m->c = 2.0L * acosl(-1.0L);
    return m;
}

minima *get_known_minima (int n) {
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 1;
    o->min = get_point(n); CHECK(o->min);
    for (int i = 0; i < n; i++) {
        o->min->x[i] = 0.0L;
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
    real sum_sqr = 0.0L;
    real sum_cos = 0.0L;
    for (int i = 0; i < n; i++) {
        sum_sqr += SQR(p->x[i]);
        sum_cos += cosl(m->c * p->x[i]);
    }
    p->f = -m->a * expl(-m->b * sqrtl(sum_sqr / n)) - expl(sum_cos / n) + m->a + expl(1.0L);
}
