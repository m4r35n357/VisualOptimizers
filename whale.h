
#include "base.h"
#include "model.h"

whale *create_whale (int dim, real min_x, real max_x, model *m);

bool woa (point *solution, int max_i, int n, int dim, real min_x, real max_x, model *m);

int randint (int n);

real randreal (void);
