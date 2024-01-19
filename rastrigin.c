/*
 * Rastrigin function https://www.sfu.ca/~ssurjano/rastr.html
 */
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real A, PI; };

model *get_parameters () {
    model *m = malloc(sizeof (model));
    m->A = 10.0L;
    m->PI = acosl(-1.0L);
    return m;
}

void cost (int n, point *p, const model *m) { (void)m;
    real value = m->A * n;
    for (int i = 0; i < n; i++) {
        value += SQR(p->x[i]) - m->A * cosl(2.0L * m->PI * p->x[i]);
    }
    p->f = value;
}
