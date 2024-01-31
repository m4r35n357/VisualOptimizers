#pragma once

#include "base.h"
#include "model.h"

typedef struct Options {
    int dim;   // number of dimensions
    int whales;   // number of whales
    int iterations;   // maximum number of allowed iterations
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    bool step_mode; // free-running or single-step algorithm
} options;

/*
 * Options
 */
options get_options (char **, bool);

typedef struct Population {
    whale **whales;
    whale *Xp;
    real *X_next;
    bool looping;
} population;

whale *get_whale (int dim, real min_x, real max_x, model *m);

population *get_population (real min_x, real max_x, model *m, options o);

bool woa (population *whales, point *solution, real min_x, real max_x, model *m, options o);

int randint (int n);

real randreal (void);
