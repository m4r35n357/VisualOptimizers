#pragma once

#include "base.h"
#include "model.h"

/*
 * Optimizer settings
 */
typedef struct Optimset {
    real tolerance;    // tolerance on the function value
    int max_iterations;   // maximum number of allowed iterations
    int adaptive;  // simplex updates reduced for dimension > 2
    real size;  // size of initial simplex
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    bool step_mode; // free-running or single-step algorithm
} optimset;

/*
 * Optimizer settings
 */
optimset get_settings (char **, bool);

/*
 * The "simplex" containing an array of n + 1 points each of dimension n
 */
typedef struct Simplex {
    point *p, *reflect, *centroid, *trial;
    int n, iterations, evaluations;
    bool looping;
    real delta_x, delta_f;
} simplex;

/*
 * "Simplex" or "Amoeba" optimizer
 */
bool nelder_mead (simplex *, const model *, const optimset *);

/*
 * Utility functions
 */
point *get_point (int);

simplex *get_simplex (int, real, const point *);

real distance (int, const point *, const point *);

int compare (const void *, const void *);

void sort (simplex *);

void project (point *, simplex *, const model *, real, const point *, const point *);

void copy_point (int, const point *, point *);

void print_point (int, const point *, int, int);
