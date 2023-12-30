
#include <stdlib.h>
#include "model.h"

struct Parameters { real a, b; };

static void rosenbrock (point *p, int n, const parameters *m) { (void)n;
    p->f = SQR(m->a - p->x[0]) + m->b * SQR(p->x[1] - SQR(p->x[0]));
}

model *get_model () {
    model *m = malloc(sizeof (model));
    m->p = malloc(sizeof (parameters));
    m->p->a = 1.0L;
    m->p->b = 100.0L;
    m->cost = rosenbrock;
    return m;
}
