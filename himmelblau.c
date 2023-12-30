
#include <stdlib.h>
#include "model.h"

struct Parameters { real a, b; };

static void cost (int n, point *p, const parameters *m) { (void)n;
    p->f = SQR(SQR(p->x[0]) + p->x[1] - m->a) + SQR(p->x[0] + SQR(p->x[1]) - m->b);
}

model *get_model () {
	model *m = malloc(sizeof (model));
    m->p = malloc(sizeof (parameters));
    m->p->a = 11.0L;
    m->p->b = 7.0L;
    m->c = cost;
    return m;
}
