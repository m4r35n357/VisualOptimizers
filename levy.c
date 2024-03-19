/*
 * Levy function https://www.sfu.ca/~ssurjano/levy.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b, PI; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
    m->b = 100.0L;
    m->PI = acosl(-1.0L);
    return m;
}

minima *get_known_minima () {
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min[0].x);
    m->min[0].x[0] = 1.0L;
    m->min[0].x[1] = 1.0L;
    m->min[0].x[2] = 1.0L;
    m->min[0].f = 0.0L;
    return m;
}

static real omega (real x) {
    return 1.0L + (x - 1.0L) / 4.0L;
}

void cost (int n, point *p, const model *m) {
    real omega_n_1 = omega(p->x[n - 1]);
    p->f = SQR(sinl(m->PI * omega(p->x[0]))) + SQR(omega_n_1 - 1.0L) * (1.0L + SQR(sinl(2.0L * m->PI * omega_n_1)));
    for (int i = 0; i < n - 1; i++) {
        real omega_i = omega(p->x[i]);
        p->f += SQR(omega_i - 1.0L) * (1.0L + 10.0L * SQR(sinl(m->PI * omega_i + 1.0L)));
    }
}
