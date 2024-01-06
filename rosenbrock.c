
#include <stdlib.h>
#include "model.h"

struct Model { real a, b; };

model *get_parameters () {
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
    m->b = 100.0L;
    return m;
}

void cost (int n, point *p, const model *m) {
    real sum = 0.0L;
    for (int i = 0; i < n - 1; i++) {
        sum += SQR(m->a - p->x[i]) + m->b * SQR(p->x[i + 1] - SQR(p->x[i]));
    }
    p->f = sum;
}
