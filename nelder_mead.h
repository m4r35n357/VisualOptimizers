
#pragma once

#include "main.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define BASE 10

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

void sort(simplex *);

void get_centroid(const simplex *, point *);

int processing(const simplex *, int, int, const optimset *);

void project(const simplex *, const point *, double, point *);

void swap_points(point *, point *);

void copy_point(int, const point *, point *);

void print_point(int, const point *);
