
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Parameters { real _; };

real alpha[4] = {1.0L, 1.2L, 3.0L, 3.2L};

real A[4][3] = {{3.0L, 10.0L, 30.0L},
                  {0.1L, 10.0L, 35.0L},
                  {3.0L, 10.0L, 30.0L},
                  {0.1L, 10.0L, 35.0L}};

real P[4][3] = {{0.3689L, 0.1170L, 0.2673L},
                  {0.4699L, 0.4387L, 0.7470L},
                  {0.1091L, 0.8732L, 0.5547L},
                  {0.0381L, 0.5743L, 0.8828L}};

static void hartmann3 (point *p, int n, const parameters *m) { (void)n; (void)m;
    real outer = 0.0L;
    for (int i = 0; i < 4; i++) {
        real inner = 0.0L;
        for (int j = 0; j < 3 ; j++) {
            inner -= A[i][j] * SQR(p->x[j] - P[i][j]);
        }
        outer -= alpha[i] * expl(inner);
    }
    p->f = outer;
}

model *get_model () {
    model *m = malloc(sizeof (model));
    m->p = NULL;
    m->cost = hartmann3;
    return m;
}
