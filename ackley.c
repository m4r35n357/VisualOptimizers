
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { double a, b, c; };

model *get_parameters () {
    model *m = malloc(sizeof (model));
    m->a = 20.0;
    m->b = 0.2;
    m->c = 2.0 * acos(-1.0);
    return m;
}

//-----------------------------------------------------------------------------
// Ackley function
// - n is the dimension of the data
// - point is the location where the function will be evaluated
// - arg contains the parameters of the function
// More details on the function at http://www.sfu.ca/%7Essurjano/ackley.html
//-----------------------------------------------------------------------------
void cost (int n, point *p, const model *m) {
    // cost function computation for arguments of exp
    double sum_sqr = 0;
    double sum_cos = 0;
    for (int i = 0; i < n; i++) {
        sum_sqr += SQR(p->x[i]);
        sum_cos += cos(m->c * p->x[i]);
    }
    // final result
    p->fx = -m->a * exp(-m->b * sqrt(sum_sqr / n)) - exp(sum_cos / n) + m->a + exp(1.0);
}
