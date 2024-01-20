/*
 * Hartmann 3-Dimensional function https://www.sfu.ca/~ssurjano/hart3.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real _; };

real alpha[4] = {1.0L, 1.2L, 3.0L, 3.2L};

real A[4][3] = {{3.0L, 10.0L, 30.0L},
                  {0.1L, 10.0L, 35.0L},
                  {3.0L, 10.0L, 30.0L},
                  {0.1L, 10.0L, 35.0L}};

real P[4][3] = {{0.3689L, 0.1170L, 0.2673L},
                  {0.4699L, 0.4387L, 0.7470L},
                  {0.1091L, 0.8732L, 0.5547L},
                  {0.0381L, 0.5743L, 0.8828L}};

model *model_init () {
    return NULL;
}

minima *get_known_minima () {
    minima *m = malloc(sizeof (minima)); CHECK(m);
    m->n_minima = 1;
    m->min = malloc((size_t)m->n_minima * sizeof (point)); CHECK(m->min);
    m->min[0].x = malloc((size_t)3 * sizeof (real)); CHECK(m->min->x);
    m->min[0].x[0] = 0.114614L;
    m->min[0].x[1] = 0.555649L;
    m->min[0].x[2] = 0.852547L;
    m->min[0].f = -3.86278L;
    return m;
}

void cost (int n, point *p, const model *m) { (void)n; (void)m;
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
