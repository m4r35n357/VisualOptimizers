
#include <stdlib.h>
#include "main.h"

struct Model { double a, b; };

model *get_parameters () {
    model *m = malloc(sizeof (model));
    m->a = 11.0;
    m->b = 7.0;
    return m;
}

void cost (int n, point *p, const model *m) { (void)n;
  p->fx = SQR(SQR(p->x[0]) + p->x[1] - m->a) + SQR(p->x[0] + SQR(p->x[1]) - m->b);
}
