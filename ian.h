#ifndef IAN_H
#define IAN_H

#pragma once

#include "point.h"

typedef struct {
  double a;
  double b;
  double c;
} ian_param_t;

void ian_fun(int, point_t *, const void *);

#endif
