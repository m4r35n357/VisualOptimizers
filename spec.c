/*
 * RF low-pass filter - from specifications
 */
#include <math.h>
#include "model.h"

struct Model { real pb, sb, ksi, min; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->pb = 0.99L;  // passband transmission: 0.99 == -0.04dB
    m->sb = 0.01L;  // stopband transmission: 0.01 == -20dB
    m->ksi = 1.5L;  // selectivity (>1.0)
    m->min = 0.0L;  // inequality constraint - passive components!
    return m;
}

minima *get_known_minima (int n) { (void)n;
    return NULL;
}

void cost (int n, point *p, const model *m) {
    for (int i = 0; i < n; i++) {
        if (p->x[i] < m->min) {
            p->f = INFINITY;
            return;
        }
    }
    p->f = 0.0L;
    for (int i = 0; i <= 50; i++) {
        real t = tx(n, p, powl(10.0L, 0.02L * i - 1.0L)); // up to w == 1.0
        p->f += t >= m->pb ? 0.0L : SQR(m->pb - t);
    }
    for (int i = 0; i <= 50; i++) {
        real t = tx(n, p, powl(10.0L, 0.02L * i) + m->ksi - 1.0L); // w * ksi upwards
        p->f += t <= m->sb ? 0.0L : SQR(t - m->sb);
    }
}
