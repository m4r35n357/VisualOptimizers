
#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main(int argc, const char *argv[]) {
  PRINT_ARGS(argc, argv);
  CHECK(argc > 2);

  // reading initial point from command line
  const int n = argc - 2;
  point start;
  start.x = malloc((size_t)n * sizeof(double));  CHECK(start.x);
  for (int i = 0; i < n; i++) {
    start.x[i] = strtod(argv[i + 2], NULL);
  }

  // optimisation settings
  optimset opt = {
    .tolx = 1.0e-9,
    .tolf = 1.0e-9,
	.max_iter = 5000,
	.max_eval = 5000,
	.verbose = (int)strtol(argv[1], NULL, BASE)
  }; CHECK(opt.verbose == 0 || opt.verbose == 1);

  // cost function parameters
  model *m = get_parameters();

  // call optimization method
  point solution;
  nelder_mead(n, &start, &solution, m, &opt);

  // evaluate and print starting point
  printf("%sInitial point ", GRY);
  cost(n, &start, m);
  print_point(n, &start);
  // print solution
  printf("     %sSolution ", GRY);
  print_point(n, &solution);

  // free memory
  free(start.x);
  free(solution.x);

  return 0;
}
