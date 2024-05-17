/*
 * Schwefel function https://www.sfu.ca/~ssurjano/schwef.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real min_edge, max_edge, *shift; };

model *model_init (int n) {
    model *m = malloc(sizeof (model));
    m->min_edge = -500.0L;  // inequality constraint
    m->max_edge = 500.0L;  // inequality constraint
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
        o->min->x[i] = 420.9687L + m->shift[i];
    }
    o->min->f = 0.0L;
    return o;
}

void cost (int n, point *p, const model *m) {
    p->f = 418.9829L * n;
    for (int i = 0; i < n; i++) {
        real xi = p->x[i] - m->shift[i];
        if (xi <= m->min_edge || xi >= m->max_edge) {
            p->f = INFINITY;
            break;
        }
        p->f -= xi * sinl(sqrtl(fabsl(xi)));
    }
}
