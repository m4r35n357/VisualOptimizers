/*
 * Styblinski-Tang function https://www.sfu.ca/~ssurjano/stybtang.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->a = 16.0L;
    m->b = 5.0L;
    return m;
}

minima *get_known_minima () {
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min[0].x);
    m->min[0].x[0] = -1.903534L;
    m->min[0].x[1] = -0.903534L;
    m->min[0].x[2] = 0.096466L;
    m->min[0].f = -117.498497L;
    return m;
}

void cost (int n, point *p, const model *m) { (void)m;
    real sum = 0.0L;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - (real)(i + 1);
        real sqr = SQR(xi);
        sum += SQR(sqr) - m->a * sqr + m->b * xi;
    }
    p->f = 0.5L * sum;
}
