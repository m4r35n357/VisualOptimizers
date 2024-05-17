/*
 * Levy function https://www.sfu.ca/~ssurjano/levy.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b, PI, *shift; };

model *model_init (int n) {
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
    m->b = 100.0L;
    m->PI = acosl(-1.0L);
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
        o->min->x[i] = 1.0L + m->shift[i];
    }
    o->min->f = 0.0L;
    return o;
}

static real omega (real x) {
    return 1.0L + (x - 1.0L) / 4.0L;
}

void cost (int n, point *p, const model *m) {
    real omega_0 = omega(p->x[0] - m->shift[0]);
    real omega_n_1 = omega(p->x[n - 1] - m->shift[n - 1]);
    p->f = SQR(sinl(m->PI * omega_0)) + SQR(omega_n_1 - 1.0L) * (1.0L + SQR(sinl(2.0L * m->PI * omega_n_1)));
    for (int i = 0; i < n - 1; i++) {
        real omega_i = omega(p->x[i] - m->shift[i]);
        p->f += SQR(omega_i - 1.0L) * (1.0L + 10.0L * SQR(sinl(m->PI * omega_i + 1.0L)));
    }
}
