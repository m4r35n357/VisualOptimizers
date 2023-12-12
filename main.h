
#pragma once

#include "point.h"

#define PI 3.1415926535897932384626433832795

#define SQR(x) ((x) * (x))

// define optimization settings
typedef struct {
  double tolx;    // tolerance on the simplex solutions coordinates
  double tolf;    // tolerance on the function value
  int max_iter;   // maximum number of allowed iterations
  int max_eval;   // maximum number of allowed function evaluations
  int verbose;    // toggle verbose output during minimization
} optimset_t;

typedef struct Parameters model;

model *get_parameters(void);

//-----------------------------------------------------------------------------
// Cost function interface
//-----------------------------------------------------------------------------

void function (int, point *, const model *);
