/*
 * Artificial Neural Network simulating OR gate function
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b, min_edge, max_edge; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->min_edge = -5.0L;  // inequality constraint
    m->max_edge =  5.0L;  // inequality constraint
    return m;
}

minima *get_known_minima (int n) { (void)n;
    return NULL;
}

static real nn (real i1, real w1, real i2, real w2, real b) {
    real tmp = i1 * w1 + i2 * w2 - b;
    return tmp > 0.0L ? 1.0L : 0.0L;
}

void cost (int n, point *p, const model *m)  { (void)n; (void)m;
    for (int i = 0; i < n; i++) {
        if (p->x[i] <= m->min_edge || p->x[i] >= m->max_edge) {
            p->f = INFINITY;
            return;
        }
    }
    p->f = 0.0L;

    p->f += SQR(0.0L - nn(0.0L, p->x[0], 0.0L, p->x[1], p->x[2]));
    p->f += SQR(1.0L - nn(0.0L, p->x[0], 1.0L, p->x[1], p->x[2]));
    p->f += SQR(1.0L - nn(1.0L, p->x[0], 0.0L, p->x[1], p->x[2]));
    p->f += SQR(1.0L - nn(1.0L, p->x[0], 1.0L, p->x[1], p->x[2]));

    p->f += SQR(0.0L - nn(0.2L, p->x[0], 0.2L, p->x[1], p->x[2]));
    p->f += SQR(1.0L - nn(0.2L, p->x[0], 0.8L, p->x[1], p->x[2]));
    p->f += SQR(1.0L - nn(0.8L, p->x[0], 0.2L, p->x[1], p->x[2]));
    p->f += SQR(1.0L - nn(0.8L, p->x[0], 0.8L, p->x[1], p->x[2]));
}
