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

//-----------------------------------------------------------------------------
// Ackley function
// - n is the dimension of the data
// - point is the location where the function will be evaluated
// - arg contains the parameters of the function
// More details on the function at http://www.sfu.ca/%7Essurjano/ackley.html
//-----------------------------------------------------------------------------

void function (int n, point_t *point, const void *arg) {
  // cast the void pointer to what we expect to find
  const model *params = (const model *)arg;

  // cost function computation for arguments of exp
  double sum_squares = 0;
  double sum_cos = 0;
  for (int i = 0; i < n; i++) {
    sum_squares += SQUARE(point->x[i]);
    sum_cos += cos(params->c * point->x[i]);
  }

  // final result
  point->fx = -params->a * exp(-params->b * sqrt(sum_squares / n)) -
              exp(sum_cos / n) + params->a + exp(1.0);
}
