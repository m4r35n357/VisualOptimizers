/*
 * Dixon-Price function https://www.sfu.ca/~ssurjano/dixonpr.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
    m->b = 2.0L;
    return m;
}

minima *get_known_minima (int n, const model *m) { (void)m;
    real y = 0.5L * sqrtl(2.0L);
    real z = powl(2.0L, -0.75L);
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 4;
    o->min = malloc((size_t)o->n_minima * sizeof (point)); CHECK(o->min);
    o->min[0].x = malloc((size_t)n * sizeof (real)); CHECK(o->min[0].x);
    o->min[0].x[0] = 1.0L;
    o->min[0].x[1] = y;
    o->min[0].x[2] = z;
    o->min[0].f = 0.0L;
    o->min[1].x = malloc((size_t)n * sizeof (real)); CHECK(o->min[1].x);
    o->min[1].x[0] = 1.0L;
    o->min[1].x[1] = -y;
    o->min[1].x[2] = z;
    o->min[1].f = 0.0L;
    o->min[2].x = malloc((size_t)n * sizeof (real)); CHECK(o->min[2].x);
    o->min[2].x[0] = 1.0L;
    o->min[2].x[1] = y;
    o->min[2].x[2] = -z;
    o->min[2].f = 0.0L;
    o->min[3].x = malloc((size_t)n * sizeof (real)); CHECK(o->min[3].x);
    o->min[3].x[0] = 1.0L;
    o->min[3].x[1] = -y;
    o->min[3].x[2] = -z;
    o->min[3].f = 0.0L;
    return o;
}

void cost (int n, point *p, const model *m) {
    p->f = SQR(p->x[0] - m->a);
    for (int i = 1; i < n; i++) {
        p->f += (i + 1) * SQR(m->b * SQR(p->x[i]) - p->x[i - 1]);
    }
}
