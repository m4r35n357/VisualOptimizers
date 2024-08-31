#pragma once

#include "model.h"

/*
 * Optimizer settings
 */
typedef struct Optimset {
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    int n;   // number of dimensions
    real tolerance;    // tolerance on the function value
    int max_evaluations;   // maximum number of allowed evaluations
    real size;  // size of initial simplex
    bool adaptive;  // simplex updates smaller for higher dimensions
    int init_mode;  // 0 for explicit coordinates, 1 for random initialization, 2 for "bulk mode"
    real lower, upper;  // range for coordinates of initial point
    bool step_mode; // free-running or single-step algorithm
} optimset;

/*
 * Optimizer settings
 */
optimset get_settings (char **);

/*
 * The "simplex" containing an array of n + 1 points each of dimension n
 */
typedef struct Simplex {
    point *p, *reflect, *centroid, *trial;
    int iterations, evaluations;
    bool looping;
    real ALPHA, GAMMA, RHO, SIGMA, delta_x, delta_f;
} simplex;

/*
 * "Simplex" or "Amoeba" optimizer
 */
bool nelder_mead (simplex *, const model *, const optimset *);

/*
 * Multidirectional Search Optimizer
 */
bool multidirectional_search (simplex *, const model *, const optimset *);

/*
 * Utility functions
 */
void set_simplex (simplex *, int, real, const point *, const model *);

simplex *get_simplex (int, real, const point *, const model *, bool);

int compare (const void *, const void *);

void sort (simplex *, int);

void project (point *, simplex *, int, const model *, real, const point *, const point *);
