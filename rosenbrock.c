
#include <stdlib.h>
#include "model.h"

struct Model { real a, b; };

model *get_parameters () {
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
    m->b = 100.0L;
    return m;
}

void cost (int n, point *p, const model *m) { (void)n;
    p->f = SQR(m->a - p->x[0]) + m->b * SQR(p->x[1] - SQR(p->x[0]));
}
