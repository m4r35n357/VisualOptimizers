
#include <stdlib.h>
#include "model.h"

struct Model { real a, b; };

model *get_parameters () {
    model *m = malloc(sizeof (model));
    m->a = 11.0L;
    m->b = 7.0L;
    return m;
}

void cost (int n, point *p, const model *m) { (void)n;
    p->f = SQR(SQR(p->x[0]) + p->x[1] - m->a) + SQR(p->x[0] + SQR(p->x[1]) - m->b);
}
