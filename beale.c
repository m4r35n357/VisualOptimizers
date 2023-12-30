
#include <stdlib.h>
#include "model.h"

struct Parameters { real a, b, c; };

static void cost (int n, point *p, const parameters *m) { (void)n;
    p->f = SQR(m->a - p->x[0] + p->x[0] * p->x[1]) \
         + SQR(m->b - p->x[0] + p->x[0] * SQR(p->x[1])) \
         + SQR(m->c - p->x[0] + p->x[0] * p->x[1] * SQR(p->x[1]));
}

model *get_model () {
	model *m = malloc(sizeof (model));
    m->p = malloc(sizeof (parameters));
    m->p->a = 1.5L;
    m->p->b = 2.25L;
    m->p->c = 2.625L;
    m->c = cost;
    return m;
}
