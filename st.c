/*
 * Styblinski-Tang function https://www.sfu.ca/~ssurjano/stybtang.html
 */
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->a = 16.0L;
    m->b = 5.0L;
    return m;
}

minima *get_known_minima () {
    return NULL;
}

void cost (int n, point *p, const model *m) { (void)m;
    real sum = 0.0L;
    for (int i = 0; i < n; i++) {
        real sqr = SQR(p->x[i]);
        sum += SQR(sqr) - m->a * sqr + m->b * p->x[i];
    }
    p->f = 0.5L * sum;
}
