#pragma once

#include "model.h"

typedef struct Config {
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    int n;   // number of dimensions
    int m;   // number of points
    int k_max;   // maximum number of allowed iterations
    int convergence;  // strategy
    int nelder_mead;   // add an NM convergence step
    real lower;   // bottom of coordinate range
    real upper;   // top of coordinate range
    bool step_mode; // free-running or single-step algorithm
} config;

/*
 * Options
 */
config get_config (char **, bool);

typedef struct Spiral {
    point **p;
    point *centre, *best, *update;
    int k, k_star, evaluations;
    real r;
    bool looping, dual_mode;
} spiral;

point *get_spiral_point (spiral *s, model *m, config c);

spiral *get_spiral (model *m, config c);

bool soa (spiral *s, model *m, config c);
