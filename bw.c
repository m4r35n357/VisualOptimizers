/*
 * RF low-pass filter - Butterworth
 */
#include <math.h>
#include <complex.h>
#include "model.h"

struct Model { void *_; };

model *model_init (int n) { (void)n;
    return NULL;
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

static real tx (int n, point *p, real w) {
    long double complex g = 1.0L + I * w * p->x[0];
    for (int r = 1; r < n; r++) {
        g = 1.0L / g + I * w * p->x[r];
    }
    for (int r = n - 2; r >= 0; r--) {
        g = 1.0L / g + I * w * p->x[r];
    }
    return 1.0L - SQR(cabsl((g - 1.0L) / (g + 1.0L)));
}

void cost (int n, point *p, const model *m) { (void)m;
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= 0.0L) {  // positive component values!
            p->f = INFINITY;
            return;
        }
    }
    p->f = 0.0L;
    for (int i = 0; i <= 100; i++) {
        real w = powl(10.0L, 0.02L * i - 1.0L);
        p->f += SQR(1.0L / (1.0L + powl(w, 2 * (2 * n - 1))) - tx(n, p, w));
    }
}
