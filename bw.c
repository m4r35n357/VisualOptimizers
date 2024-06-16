/*
 * RF low-pass filter - Butterworth
 */
#include <math.h>
#include "model.h"

struct Model { real min; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->min = 0.0L;  // inequality constraint - passive components!
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
    return SQR(1.0L / (1.0L + powl(w, 2 * (2 * n - 1))) - tx(n, p, w));
}

void cost (int n, point *p, const model *m) {
    for (int i = 0; i < n; i++) {
        if (p->x[i] < m->min) {
            p->f = INFINITY;
            return;
        }
    }
    p->f = 0.0L;
    for (int i = 0; i <= 100; i++) {
        p->f += error(n, p, powl(10.0L, 0.02L * i - 1.0L));
    }
}
