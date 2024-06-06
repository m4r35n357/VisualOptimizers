/*
 * LC low-pass filter
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "model.h"

typedef long double complex cplx;

struct Model { real min_edge, max_edge; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->min_edge = 0.0L;  // inequality constraint
    m->max_edge = 5.0L;  // inequality constraint
    return m;
}

minima *get_known_minima (int n) { (void)n;
    real PI = acosl(-1.0L);
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 1;
    o->min = get_point(n); CHECK(o->min);
    for (int r = 0; r < n; r++) {
        o->min->x[r] = 2.0L * sinl((2.0L * (r + 1.0L) - 1.0L) * PI / (2 * (2 * n - 1)));
    }
    o->min->f = 0.0L;
    return o;
}

static real error (int n, point *p, real w) {
    cplx g = 1.0L + I * w * p->x[0];
    for (int r = 1; r < n; r++) {
        g = 1.0L / g + I * w * p->x[r];
    }
    for (int r = n - 2; r >= 0; r--) {
        g = 1.0L / g + I * w * p->x[r];
    }
    return SQR(1.0L - SQR(cabsl((g - 1.0L) / (g + 1.0L))) - 1.0L / (1.0L + powl(w, 2 * (2 * n - 1))));
}

void cost (int n, point *p, const model *m) {
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= m->min_edge || p->x[i] >= m->max_edge) {
            p->f = INFINITY;
            return;
        }
    }
    p->f = 0.0L;
    for (int i = 0; i <= 50; i++) {
        p->f += error(n, p, i * 0.1L);
    }
    p->f += error(n, p, 10.0L);
    p->f += error(n, p, 100.0L);
    p->f += error(n, p, 1000.0L);
    p->f += error(n, p, 10000.0L);
    p->f += error(n, p, 100000.0L);
    p->f += error(n, p, 1000000.0L);
}
