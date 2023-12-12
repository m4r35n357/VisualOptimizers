#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "point.h"

struct Parameters { double a, b, c; };

model *get_parameters () {
    return NULL;
}

void function (int n, point_t *point, const void *arg) { (void)arg;
  // cost function computation for arguments of exp
  double sum_squares = 0;
  for (int i = 0; i < n; i++) {
    sum_squares += SQUARE(point->x[i]);
  }

  // final result
  point->fx = sqrt(sum_squares / n);
}
