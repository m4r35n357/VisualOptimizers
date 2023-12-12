
#pragma once

#include "point.h"

#define PI 3.1415926535897932384626433832795

#define SQR(x) ((x) * (x))

typedef struct Parameters model;

model *get_parameters(void);

//-----------------------------------------------------------------------------
// Cost function interface
//-----------------------------------------------------------------------------

void function (int, point *, const model *);
