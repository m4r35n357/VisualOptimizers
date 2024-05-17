/*
 * Michalewicz function https://www.sfu.ca/~ssurjano/michal.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real m, PI, min_edge, max_edge; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->m = 1.0L;
    m->PI = acosl(-1.0L);
    m->min_edge = 0.0L;  // inequality constraint
    m->max_edge = m->PI;  // inequality constraint
    return m;
}

minima *get_known_minima (int n, const model *m) { (void)m;
    minima *o = malloc(sizeof (minima)); CHECK(o);
    o->n_minima = 1;
    o->min = get_point(n); CHECK(o->min);
    o->min->x[0] = 2.071689L;
    o->min->x[1] = 1.570796L;
    o->min->x[2] = 1.304668L;
    o->min->f = -2.802848L;
    return o;
}

void cost (int n, point *p, const model *m) {
    p->f =0.0L;
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= m->min_edge || p->x[i] >= m->max_edge) {
            p->f = INFINITY;
            break;
        }
        p->f -= sinl(p->x[i]) * powl(sinl((i + 1) * SQR(p->x[i]) / m->PI), 2.0L * m->m);
    }
}
