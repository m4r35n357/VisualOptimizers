#pragma once

#include "base.h"
#include "model.h"

typedef struct Config {
    int n;   // number of dimensions
    int m;   // number of points
    int k_max;   // maximum number of allowed iterations
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    bool step_mode; // free-running or single-step algorithm
} config;

/*
 * Options
 */
options get_config (char **, bool);

typedef struct Spiral {
    point **points;
    point *x_star;
    real **R;
    int n, iterations, evaluations;
    bool looping;
} spiral;

whale *get_point (int dim, real min_x, real max_x, model *m);

population *get_spiral (real min_x, real max_x, model *m, config c);

bool soa (spiral *s, point *solution, real min_x, real max_x, model *m, config c);

int randint (int n);

real randreal (void);
