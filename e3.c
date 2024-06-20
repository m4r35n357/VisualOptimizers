/*
 * 3rd order "elliptic" (notch) filter from specifications
 *    ----C(2)---
 *    |         |
 * o------L(1)-----o   or its "dual"
 *    |         |
 *   C(0)      C(0)
 *    |         |
 * o---------------o
 */
#include <math.h>
#include <complex.h>
#include "model.h"

struct Model { real pb, sb, ksi, min, max; };

model *model_init (int n) { (void)n;
    CHECK(n == 3);
    bool override = false;
    char *datafile = "e3specs.txt";
    real pb, sb, ksi;
    FILE *specs = fopen(datafile, "r");
    if (specs) override = fscanf(specs, "%Le %Le %Le", &pb, &sb, &ksi) == 3;
    if (override) fprintf(stderr, " %sOverrides (%s%s%s): ripple = %s%.*Lf%s, loss = %s%.*Lf%s, selectivity = %s%.*Lf%s\n",
                          GRY, NRM, datafile, GRY, NRM, 3, pb, GRY, NRM, 6, sb, GRY, NRM, 3, ksi, GRY);
    model *m = malloc(sizeof (model));
    m->pb = override ? pb : 0.8L;  // passband transmission: 0.8 == -0.969dB
    m->sb = override ? sb : 0.0025L;// stopband transmission: 0.0025 == -26dB
    m->ksi = override ? ksi : 1.5L;  // selectivity (>1.0)
    m->min = 0.0L;  // inequality constraint - passive components!
    m->max = 10.0L;  // inequality constraint - in case values blow up
    return m;
}

minima *get_known_minima (int n) { (void)n;
    return NULL;
}

static real e3 (int n, point *p, real omega) { (void)n;
    long double complex g = 1.0L + I * omega * p->x[0];
    g = 1.0L / g + 1.0L / (I * omega * p->x[2] + 1.0L / (I * omega * p->x[1]));
    g = 1.0L / g + I * omega * p->x[0];
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
        real t = e3(n, p, powl(10.0L, 0.02L * i - 1.0L)); // up to w == 1.0
        p->f += t >= m->pb ? 0.0L : SQR(m->pb - t);
    }
    for (int i = 0; i <= 50; i++) {
        real t = e3(n, p, powl(10.0L, 0.02L * i) + m->ksi - 1.0L); // w * ksi upwards
        p->f += t <= m->sb ? 0.0L : SQR(m->sb - t);
    }
}
