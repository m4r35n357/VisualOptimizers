#include <stdio.h>
#include <string.h>
#include "point.h"

//-----------------------------------------------------------------------------
// Utilities for point_t
//-----------------------------------------------------------------------------

void swap_points(point_t *p1, point_t *p2) {
  point_t _ = *p1;
  *p1 = *p2;
  *p2 = _;
}

void copy_point(int n, const point_t *src, point_t *dst) {
  memcpy(dst->x, src->x, sizeof(double) * (size_t)n);
  dst->fx = src->fx;
}

void print_point(int n, const point_t *point) {
  printf("x = [ ");
  for (int i = 0; i < n; i++) {
    printf("%.6e ", point->x[i]);
  }
  printf("], fx = %.6e \n", point->fx);
}
