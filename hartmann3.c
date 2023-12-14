
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { double a; };

double alpha[4] = {1.0, 1.2, 3.0, 3.2};

double A[4][3] = {{3.0, 10.0, 30.0},
                  {0.1, 10.0, 35.0},
                  {3.0, 10.0, 30.0},
                  {0.1, 10.0, 35.0}};

double P[4][3] = {{0.3689, 0.1170, 0.2673},
                  {0.4699, 0.4387, 0.7470},
                  {0.1091, 0.8732, 0.5547},
                  {0.0381, 0.5743, 0.8828}};

model *get_parameters () {
    return NULL;
}

void cost (int n, point *p, const model *m) { (void)n; (void)m;
  double outer = 0.0;
  for (int i = 0; i < 4; i++) {
	  double inner = 0.0;
	  for (int j = 0; j < 3 ; j++) {
		  inner -= A[i][j] * SQR(p->x[j] - P[i][j]);
	  }
	  outer -= alpha[i] * exp(inner);
  }
  p->fx = outer;
}
