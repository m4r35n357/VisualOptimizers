/*
 * Styblinski-Tang function https://www.sfu.ca/~ssurjano/stybtang.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b, *shift; };

model *model_init (int n) {
    model *m = malloc(sizeof (model));
    m->a = 16.0L;
    m->b = 5.0L;
    m->shift = malloc((size_t)n * sizeof (real)); CHECK(m->shift);
    for (int i = 0; i < n; i++) {
        m->shift[i] = (real)(i + 1);
    }
    return m;
}

minima *get_known_minima (int n, const model *m) { (void)m;
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
    real sum = 0.0L;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - m->shift[i];
        real sqr = SQR(xi);
        sum += SQR(sqr) - m->a * sqr + m->b * xi;
    }
    p->f = 0.5L * sum;
}
