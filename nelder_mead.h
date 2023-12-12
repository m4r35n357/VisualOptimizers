
#pragma once

#include "main.h"
#include "point.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define RHO 1.0
#define CHI 2.0
#define GAMMA 0.5
#define SIGMA 0.5

// define a simplex struct containing an array of n+1 points (p)
// each having dimension (n)
typedef struct {
  point *p;
  int n;
} simplex_t;

//-----------------------------------------------------------------------------
// Nelder-Mead algorithm
//-----------------------------------------------------------------------------

void nelder_mead(int, const point *, point *, const model *, const optimset_t *);

//-----------------------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------------------

int compare(const void *, const void *);

void simplex_sort(simplex_t *);

void get_centroid(const simplex_t *, point *);

int continue_minimization(const simplex_t *, int, int, const optimset_t *);

void update_point(const simplex_t *, const point *, double, point *);

