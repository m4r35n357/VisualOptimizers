/*
 * Dixon-Price function https://www.sfu.ca/~ssurjano/dixonpr.html
 */
#include <stdlib.h>
#include "model.h"

struct Model { real a, b; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->a = 1.0L;
    m->b = 2.0L;
    return m;
}

minima *get_known_minima () {
    return NULL;
}

void cost (int n, point *p, const model *m) {
    p->f = SQR(p->x[0] - m->a);
    for (int i = 1; i < n; i++) {
        p->f += (i + 1) * SQR(m->b * SQR(p->x[i]) - p->x[i - 1]);
    }
}
