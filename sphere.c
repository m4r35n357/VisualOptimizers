
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Parameters { real _; };

static void sphere (point *p, int n, const parameters *m) { (void)m;
    real sum = 0.0L;
    for (int i = 0; i < n; i++) {
        sum += SQR(p->x[i]);
    }
    p->f = sqrtl(sum / n);
}

model *get_model () {
    model *m = malloc(sizeof (model));
    m->p = NULL;
    m->cost = sphere;
    return m;
}
