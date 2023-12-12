#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "nelder_mead.h"

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    printf("%s: error: not enough inputs \n", argv[0]);
    return 0;
  }

  // reading initial point from command line
  const int n = argc - 1;
  point_t start;
  start.x = malloc((size_t)n * sizeof(double));
  for (int i = 0; i < n; i++) {
    start.x[i] = atof(argv[i + 1]);
  }

  // optimisation settings
  optimset_t optimset;
  optimset.tolx = 1.0e-9;
  optimset.tolf = 1.0e-9;
  optimset.max_iter = 5000;
  optimset.max_eval = 5000;
  optimset.verbose = 1;

  // cost function parameters
  model *p = get_parameters();

  // call optimization method
  point_t solution;
  nelder_mead(n, &start, &solution, function, p, &optimset);

  // evaluate and print starting point
  printf("Initial point\n");
  function(n, &start, p);
  print_point(n, &start);
  // print solution
  printf("Solution\n");
  print_point(n, &solution);

  // free memory
  free(start.x);
  free(solution.x);

  return 0;
}
