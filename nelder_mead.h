#pragma once

#include "base.h"
#include "model.h"

#define ALPHA 1.0L
#define GAMMA 2.0L
#define RHO 0.5L
#define SIGMA 0.5L

/*
 * Optimizer settings
 */
typedef struct Optimset {
    real tolerance;    // tolerance on the function value
    int max_iterations;   // maximum number of allowed iterations
    real size;  // size of initial simplex
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
} optimset;

/*
 * Optimizer settings
 */
optimset get_settings (char **);

/*
 * "Simplex" or "Amoeba" optimizer
 */
bool nelder_mead (simplex *, point *, const model *, const optimset *);

/*
 * Utility functions
 */
point *get_point (int);

simplex *get_simplex (int, real, const point *, const model *);

real distance (int, const point *, const point *);

int compare (const void *, const void *);

void sort (simplex *);

void project (point *, simplex *, const model *, const point *, real, const point *, point *);

void copy_point (int, const point *, point *);

void print_point (int, const point *, int, int);
