#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real _; };

model *get_parameters () {
    return NULL;
}

void cost (int n, point *p, const model *m) { (void)m;
    real sum = 0.0L;
    for (int i = 0; i < n; i++) {
        sum += SQR(p->x[i]);
    }
    p->f = sqrtl(sum / n);
}
