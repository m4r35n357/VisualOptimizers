#pragma once

#include "model.h"

typedef struct Config {
    int places;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
    int n;   // number of dimensions
    int m;   // number of points
    int max_iterations;   // maximum number of allowed iterations
    real lower;   // bottom of coordinate range
    real upper;   // top of coordinate range
    bool step_mode; // free-running or single-step algorithm
} config;

/*
 * Options
 */
config get_config (char **, bool);

typedef struct Box {
    point **p;
    point *best;
    real *lower, *upper, lambda;
    int iterations, evaluations;
    bool looping;
} box;

box *get_box (model *m, config c);

bool coa (box *s, model *m, config c);
