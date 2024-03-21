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
    int max_iterations;   // maximum number of allowed iterations
    real size;  // size of initial simplex
    int adaptive;  // simplex updates reduced for dimension > 2
    int init_mode;  // 0 for explicit coordinates, 1 or more for random initialization
    real lower, upper;  // range for coordinates of initial point
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
point *get_point (int);

void set_random_coordinates (point *p, int n, real lower, real upper);

void regular_simplex (simplex *, real, const point *);

simplex *nm_simplex (int, real, const point *, bool);

real distance (int, const point *, const point *);

int compare (const void *, const void *);

void sort (simplex *);

void project (point *, simplex *, const model *, real, const point *, const point *);

void copy_point (int, const point *, point *);

void print_result (int, const point *, int, int);

void print_progress (const simplex *, const point *, int, int);
