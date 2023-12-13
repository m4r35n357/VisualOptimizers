
#pragma once

/*
 * "Low-noise" squaring for arguments with no side-effects
 */
#define SQR(x) ((x) * (x))

typedef struct Model model;

typedef struct Point {
  double *x;
  double fx;
} point;

/*
 * Initialize model
 */
model *get_parameters(void);

/*
 * Function to minimize
 */
void cost (int, point *, const model *);
