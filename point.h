
#pragma once

// define a generic point containing a position (x) and a value (fx)
typedef struct {
  double *x;
  double fx;
} point;

void swap_points(point *, point *);

void copy_point(int, const point *, point *);

void print_point(int, const point *);
