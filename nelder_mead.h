
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
typedef struct Simplex {
  point *p;
  int n;
} simplex;

//-----------------------------------------------------------------------------
// Nelder-Mead algorithm
//-----------------------------------------------------------------------------

void nelder_mead(int, const point *, point *, const model *, const optimset *);

//-----------------------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------------------

int compare(const void *, const void *);

void simplex_sort(simplex *);

void get_centroid(const simplex *, point *);

int continue_minimization(const simplex *, int, int, const optimset *);

void update_point(const simplex *, const point *, double, point *);

