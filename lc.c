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
    get_known_minima(n);
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
    fprintf(stderr, " %sExpected%s ", GRY, BLU);
    for (int r = 0; r < n; r++) {
        o->min->x[r] = 2.0L * sinl((2.0L * (r + 1.0L) - 1.0L) * PI / (2 * (2 * n - 1)));
        fprintf(stderr, "% .6Lf ", o->min->x[r]);
    }
    o->min->f = 0.0L;
    fprintf(stderr, "%s\n", NRM);
    return o;
}

static real error (int n, point *p, real w) {
    cplx G = 1.0L + I * w * p->x[0];
    for (int r = 1; r < n; r++) {
        G = 1.0L / G + I * w * p->x[r];
    }
    for (int r = n - 2; r >= 0; r--) {
        G = 1.0L / G + I * w * p->x[r];
    }
    return SQR(1.0L - SQR(cabsl((G - 1.0L) / (G + 1.0L))) - 1.0L / (1.0L + powl(w, 2 * (2 * n - 1))));
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
