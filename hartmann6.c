
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real _; };

real alpha[4] = {1.0L, 1.2L, 3.0L, 3.2L};

real A[4][6] = {{10.0L, 3.0L, 17.0L, 3.5L, 1.7L,  8.0L},
                  {0.05L, 10.0L, 17.0L, 0.1L, 8.0L, 14.0L},
                  {3.0L, 3.5L, 1.7L, 10.0L, 17.0L, 8.0L},
                  {17.0L, 8.0L, 0.05L, 10.0L, 0.1L, 14.0L}};

real P[4][6] = {{0.1312L, 0.1696L, 0.5569L, 0.0124L, 0.8283L, 0.5886L},
                  {0.2329L, 0.4135L, 0.8307L, 0.3736L, 0.1004L, 0.9991L},
                  {0.2348L, 0.1451L, 0.3522L, 0.2883L, 0.3047L, 0.6650L},
                  {0.4047L, 0.8828L, 0.8732L, 0.5743L, 0.1091L, 0.0381L}};

model *model_init () {
    return NULL;
}

minima *get_known_minima () {
    return NULL;
}

void cost (int n, point *p, const model *m) { (void)n; (void)m;
    real outer = 0.0L;
    for (int i = 0; i < 4; i++) {
        real inner = 0.0L;
        for (int j = 0; j < 6 ; j++) {
            inner -= A[i][j] * SQR(p->x[j] - P[i][j]);
        }
        outer -= alpha[i] * expl(inner);
    }
    p->f = outer;
}
