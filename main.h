#ifndef IAN_H
#define IAN_H

#pragma once

#include "point.h"

#define PI 3.1415926535897932384626433832795

typedef struct Parameters model;

model *get_parameters(void);

void function (int, point_t *, const void *);

#endif
