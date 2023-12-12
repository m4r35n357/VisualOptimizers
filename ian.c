#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "point.h"

#define SQUARE(x) ((x) * (x))

struct Parameters { double a, b, c; };

model *get_parameters () {
	model *_ = malloc(sizeof (model));
	_->a = 20.0;
	_->b = 0.2;
	_->c = 2.0 * PI;
    return _;
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
