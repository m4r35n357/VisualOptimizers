#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "point.h"

struct Parameters { double a, b, c; };

model *get_parameters () {
    return NULL;
}

void function (int n, point *p, const model *m) { (void)m;
  double sum_sqr = 0;
  for (int i = 0; i < n; i++) {
    sum_sqr += SQR(p->x[i]);
  }
  p->fx = sqrt(sum_sqr / n);
}
