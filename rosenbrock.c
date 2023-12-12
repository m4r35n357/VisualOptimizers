#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "point.h"

struct Parameters { double a, b; };

model *get_parameters () {
    model *_ = malloc(sizeof (model));
    _->a = 1.0;
    _->b = 100.0;
    return _;
}

void function (int n, point_t *point, const void *arg) { (void)n;
  // cast the void pointer to what we expect to find
  const model *params = (const model *)arg;

  // cost function computation for arguments of exp
  point->fx = SQUARE(params->a - point->x[0]) + params->b * SQUARE(point->x[1] - SQUARE(point->x[0]));
}
