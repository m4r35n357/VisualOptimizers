
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { double _; };

double alpha[4] = {1.0, 1.2, 3.0, 3.2};

double A[4][6] = {{10.0, 3.0, 17.0, 3.5, 1.7,  8.0},
                  {0.05, 10.0, 17.0, 0.1, 8.0, 14.0},
                  {3.0, 3.5, 1.7, 10.0, 17.0, 8.0},
                  {17.0, 8.0, 0.05, 10.0, 0.1, 14.0}};

double P[4][6] = {{0.1312, 0.1696, 0.5569, 0.0124, 0.8283, 0.5886},
                  {0.2329, 0.4135, 0.8307, 0.3736, 0.1004, 0.9991},
                  {0.2348, 0.1451, 0.3522, 0.2883, 0.3047, 0.6650},
                  {0.4047, 0.8828, 0.8732, 0.5743, 0.1091, 0.0381}};

model *get_parameters () {
    return NULL;
}

void cost (int n, point *p, const model *m) { (void)n; (void)m;
    double outer = 0.0;
    for (int i = 0; i < 4; i++) {
        double inner = 0.0;
        for (int j = 0; j < 6 ; j++) {
            inner -= A[i][j] * SQR(p->x[j] - P[i][j]);
        }
        outer -= alpha[i] * exp(inner);
    }
    p->f = outer;
}
