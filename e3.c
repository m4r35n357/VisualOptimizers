/*
 * 3rd order low-pass notch filter from specifications
 *        ----C(2)---
 *        |         |
 *     o------L(1)-----o---
 *        |         |     |
 * g =>  C(0)      C(0)   1    (or its "dual")
 *        |         |     |
 *     o---------------o---
 */
#include <math.h>
#include <complex.h>
#include "model.h"

struct Model { real pb, sb, ksi, min, max; };

model *model_init (int n) { (void)n;
    CHECK(n == 3);
    char *datafile = "e3specs.txt";
    real pb, sb, ksi;
    FILE *specs = fopen(datafile, "r");
    bool override = specs ? fscanf(specs, "%Le %Le %Le", &pb, &sb, &ksi) == 3 : false;
    if (specs) CHECK(!fclose(specs));
    model *m = malloc(sizeof (model));
    m->pb = override ? pb : 0.8L;  // passband transmission: 0.8 == -0.969dB
    m->sb = override ? sb : 0.0035L;// stopband transmission: 0.0035 == -24.6dB
    m->ksi = override ? ksi : 1.5L;  // selectivity (>1.0)
    fprintf(stderr, " %sSpecs (%s%s%s)  ripple %s%.*Lf%s dB, loss %s%.*Lf%s dB, selectivity %s%.*Lf%s\n",
            GRY, NRM, override ? datafile : "  DEFAULT  ", GRY,
            NRM, 1, -10.0L * log10l(m->pb), GRY, NRM, 1, -10.0L * log10l(m->sb), GRY, NRM, 1, m->ksi, GRY);
    m->min = 0.0L;  // inequality constraint - passive components!
    m->max = 10.0L;  // inequality constraint - in case values blow up
    return m;
}

minima *get_known_minima (int n) { (void)n;
    return NULL;
}

static real tx (real *x, real w) {
    long double complex g = 1.0L + I * w * x[0];
    g = 1.0L / g + 1.0L / (1.0L / (I * w * x[1]) + I * w * x[2]);
    g = 1.0L / g + I * w * x[0];
    return 1.0L - SQR(cabsl((g - 1.0L) / (g + 1.0L)));
}

void cost (int n, point *p, const model *m) {
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= m->min || p->x[i] >= m->max) {
            p->f = INFINITY;
            return;
        }
    }
    p->f = 0.0L;
    for (int i = 0; i <= 50; i++) {
        real t = tx(p->x, powl(10.0L, 0.02L * i - 1.0L)); // up to w == 1.0
        p->f += t >= m->pb ? 0.0L : SQR(m->pb - t);
    }
    for (int i = 0; i <= 50; i++) {
        real t = tx(p->x, powl(10.0L, 0.02L * i) + m->ksi - 1.0L); // w * ksi upwards
        p->f += t <= m->sb ? 0.0L : SQR(m->sb - t);
    }
}
