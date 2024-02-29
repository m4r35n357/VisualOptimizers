#pragma once

#include "model.h"

typedef struct Options {
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    int dim;   // number of dimensions
    int whales;   // number of whales
    int iterations;   // maximum number of allowed iterations
    real lower;   // bottom of coordinate range
    real upper;   // top of coordinate range
    bool step_mode; // free-running or single-step algorithm
} options;

/*
 * Options
 */
options get_options (char **, bool);

typedef struct Population {
    point **whales;
    point *prey;
    int n, iterations, evaluations;
    bool looping, restart;
} population;

population *get_population (model *m, options o);

bool woa (population *whales, model *m, options o);

real rand_real (void);
