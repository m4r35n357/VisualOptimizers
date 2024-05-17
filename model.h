#pragma once

#include "base.h"

/*
 * Model parameters
 */
typedef struct Model model;

/*
 * Optional, where known
 */
typedef struct KnownMinima {
    point *min;
    int n_minima;
} minima;

/*
 * Function to minimize
 */
void cost (int, point *, const model *);

/*
 * Initialize parameters
 */
model *model_init (int);

/*
 * Initialize minima, where known
 */
minima *get_known_minima (int, const model *);
