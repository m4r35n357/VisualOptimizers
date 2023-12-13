
#include <stdlib.h>
#include <math.h>
#include "main.h"

struct Model { double a, b, c; };

model *get_parameters () {
    return NULL;
}

void cost (int n, point *p, const model *m) { (void)m;
  double sum = 0;
  for (int i = 0; i < n; i++) {
    sum += SQR(p->x[i]);
  }
  p->fx = sqrt(sum / n);
}
