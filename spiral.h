#pragma once

#include "base.h"
#include "model.h"

typedef struct Config {
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    int n;   // number of dimensions
    real delta;  // tolerance
    int m;   // number of points
    int k_max;   // maximum number of allowed iterations
    bool step_mode; // free-running or single-step algorithm
} config;

/*
 * Options
 */
config get_config (char **, bool);

typedef struct Spiral {
    point *points;
    point *x_star, *i_b;
    real **R, delta;
    int n, k, evaluations;
    bool looping;
} spiral;

spiral *get_spiral (real min_x, real max_x, model *m, config c);

bool soa (spiral *s, model *m, config c);

int randint (int n);

real randreal (void);
