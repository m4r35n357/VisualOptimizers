
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "nelder_mead.h"

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main(int argc, const char *argv[]) {
  PRINT_ARGS(argc, argv);
  if (argc == 1) {
    printf("%s: error: not enough inputs \n", argv[0]);
    return 0;
  }

  // reading initial point from command line
  const int n = argc - 2;
  point start;
  start.x = malloc((size_t)n * sizeof(double));
  for (int i = 0; i < n; i++) {
    start.x[i] = strtod(argv[i + 2], NULL);
  }

  // optimisation settings
  optimset opt;
  opt.tolx = 1.0e-9;
  opt.tolf = 1.0e-9;
  opt.max_iter = 5000;
  opt.max_eval = 5000;
  opt.verbose = (int)strtol(argv[1], NULL, BASE);

  // cost function parameters
  model *m = get_parameters();

  // call optimization method
  point solution;
  nelder_mead(n, &start, &solution, m, &opt);

  // evaluate and print starting point
  printf("Initial point\n");
  cost(n, &start, m);
  print_point(n, &start);
  // print solution
  printf("Solution\n");
  print_point(n, &solution);

  // free memory
  free(start.x);
  free(solution.x);

  return 0;
}
