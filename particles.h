#pragma once

#include "model.h"

#define SHRINK_FACTOR 1.0e-6L

#define OMEGA 0.001L

/*
 * Options
 */
typedef struct Config {
    bool spiral; // choose between spiral and cut algorithms
    bool step_mode; // free-running or single-step ("generator") operation
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    int n;   // number of dimensions
    int m;   // number of points
    int max_iterations;   // maximum number of allowed iterations
    int mode;  // strategy
    real lower;   // bottom of coordinate range
    real upper;   // top of coordinate range
} config;

config get_config (char **);

typedef struct Population {
    point **agents;
    point *x_star, *best, *update;
    int k_star, iterations, evaluations;
    real rd, rc, *lower, *upper, lambda;
    bool looping, updated, shrinking;
} population;

population *get_spiral (model *m, config *c);

bool soa (population *s, model *m, config *c);

population *get_box (model *m, config *c);

bool coa (population *b, model *m, config *c);
