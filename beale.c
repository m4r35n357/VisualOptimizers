
#include <stdlib.h>
#include "model.h"

struct Model { double a, b, c; };

model *get_parameters () {
    model *m = malloc(sizeof (model));
    m->a = 1.5;
    m->b = 2.25;
    m->c = 2.625;
    return m;
}

void cost (int n, point *p, const model *m) { (void)n;
    p->f = SQR(m->a - p->x[0] + p->x[0] * p->x[1]) \
         + SQR(m->b - p->x[0] + p->x[0] * SQR(p->x[1])) \
         + SQR(m->c - p->x[0] + p->x[0] * p->x[1] * SQR(p->x[1]));
}
