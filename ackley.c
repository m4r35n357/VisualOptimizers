
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Parameters { real a, b, c; };

static void ackley (point *p, int n, const parameters *m) {
    // cost function computation for arguments of exp
    real sum_sqr = 0.0L;
    real sum_cos = 0.0L;
    for (int i = 0; i < n; i++) {
        sum_sqr += SQR(p->x[i]);
        sum_cos += cosl(m->c * p->x[i]);
    }
    // final result
    p->f = -m->a * expl(-m->b * sqrtl(sum_sqr / n)) - expl(sum_cos / n) + m->a + expl(1.0L);
}

model *get_model () {
    model *m = malloc(sizeof (model));
    m->p = malloc(sizeof (parameters));
    m->p->a = 20.0L;
    m->p->b = 0.2L;
    m->p->c = 2.0L * acosl(-1.0L);
    m->cost = ackley;
    return m;
}
