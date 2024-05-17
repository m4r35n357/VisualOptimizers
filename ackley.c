/*
 * Ackley function https://www.sfu.ca/~ssurjano/ackley.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b, c, *shift; };

model *model_init (int n) {
    model *m = malloc(sizeof (model));
    m->a = 20.0L;
    m->b = 0.2L;
    m->c = 2.0L * acosl(-1.0L);
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
    real sum_sqr = 0.0L;
    real sum_cos = 0.0L;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - m->shift[i];
        sum_sqr += SQR(xi);
        sum_cos += cosl(m->c * xi);
    }
    p->f = -m->a * expl(-m->b * sqrtl(sum_sqr / n)) - expl(sum_cos / n) + m->a + expl(1.0L);
}
