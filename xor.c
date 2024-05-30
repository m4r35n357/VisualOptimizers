/*
 * Artificial Neural Network simulating XOR gate function
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real min_edge, max_edge; };

model *model_init (int n) { (void)n;
    model *m = malloc(sizeof (model));
    m->min_edge = -5.0L;  // inequality constraint
    m->max_edge =  5.0L;  // inequality constraint
    return m;
}

minima *get_known_minima (int n) { (void)n;
    return NULL;
}

static real nn (real i1, real i2, real w, real b) {
    real tmp = w * (i1 + i2) - b;
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

    p->f += SQR(0.0L - nn(nn(0.0L, 0.0L, p->x[0], p->x[1]), nn(0.0L, 0.0L, p->x[2], p->x[3]), p->x[4], p->x[5]));
    p->f += SQR(1.0L - nn(nn(0.0L, 1.0L, p->x[0], p->x[1]), nn(0.0L, 1.0L, p->x[2], p->x[3]), p->x[4], p->x[5]));
    p->f += SQR(1.0L - nn(nn(1.0L, 0.0L, p->x[0], p->x[1]), nn(1.0L, 0.0L, p->x[2], p->x[3]), p->x[4], p->x[5]));
    p->f += SQR(0.0L - nn(nn(1.0L, 1.0L, p->x[0], p->x[1]), nn(1.0L, 1.0L, p->x[2], p->x[3]), p->x[4], p->x[5]));

    p->f += SQR(0.0L - nn(nn(0.2L, 0.2L, p->x[0], p->x[1]), nn(0.2L, 0.2L, p->x[2], p->x[3]), p->x[4], p->x[5]));
    p->f += SQR(1.0L - nn(nn(0.2L, 0.8L, p->x[0], p->x[1]), nn(0.2L, 0.8L, p->x[2], p->x[3]), p->x[4], p->x[5]));
    p->f += SQR(1.0L - nn(nn(0.8L, 0.2L, p->x[0], p->x[1]), nn(0.8L, 0.2L, p->x[2], p->x[3]), p->x[4], p->x[5]));
    p->f += SQR(0.0L - nn(nn(0.8L, 0.8L, p->x[0], p->x[1]), nn(0.8L, 0.8L, p->x[2], p->x[3]), p->x[4], p->x[5]));
}
