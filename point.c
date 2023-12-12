#include <stdio.h>
#include <string.h>
#include "point.h"

//-----------------------------------------------------------------------------
// Utilities for point_t
//-----------------------------------------------------------------------------

void swap_points(point *p1, point *p2) {
  point p = *p1;
  *p1 = *p2;
  *p2 = p;
}

void copy_point(int n, const point *src, point *dst) {
  memcpy(dst->x, src->x, sizeof(double) * (size_t)n);
  dst->fx = src->fx;
}

void print_point(int n, const point *p) {
  printf("x = [ ");
  for (int i = 0; i < n; i++) {
    printf("%.6e ", p->x[i]);
  }
  printf("], fx = %.6e \n", p->fx);
}
