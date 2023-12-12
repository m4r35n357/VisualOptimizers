
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "nelder_mead.h"

//-----------------------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------------------

int compare(const void *, const void *);

void simplex_sort(simplex_t *);

void get_centroid(const simplex_t *, point *);

int continue_minimization(const simplex_t *, int, int, const optimset_t *);

void update_point(const simplex_t *, const point *, double, point *);

//-----------------------------------------------------------------------------
// Main function
// - n is the dimension of the data
// - start is the initial point (unchanged in output)
// - solution is the minimizer
// - cost_function is a pointer to a fun_t type function
// - args are the optional arguments of cost_function
// - optimset are the optimisation settings
//-----------------------------------------------------------------------------

void nelder_mead(int n, const point *start, point *solution,
                 fun_t cost_function, const model *args,
                 const optimset_t *optimset) {
  // internal points
  point point_r;
  point point_e;
  point point_c;
  point centroid;

  // allocate memory for internal points
  point_r.x = malloc((size_t)n * sizeof(double));
  point_e.x = malloc((size_t)n * sizeof(double));
  point_c.x = malloc((size_t)n * sizeof(double));
  centroid.x = malloc((size_t)n * sizeof(double));

  int iter_count = 0;
  int eval_count = 0;

  // initial simplex has size n + 1 where n is the dimensionality pf the data
  simplex_t simplex;
  simplex.n = n;
  simplex.p = malloc((size_t)(n + 1) * sizeof(point));
  for (int i = 0; i < n + 1; i++) {
    simplex.p[i].x = malloc((size_t)n * sizeof(double));
    for (int j = 0; j < n; j++) {
      simplex.p[i].x[j] =
          (i - 1 == j) ? (start->x[j] != 0.0 ? 1.05 * start->x[j] : 0.00025)
                       : start->x[j];
    }
    cost_function(n, simplex.p + i, args);
    eval_count++;
  }
  // sort points in the simplex so that simplex.p[0] is the point having
  // minimum fx and simplex.p[n] is the one having the maximum fx
  simplex_sort(&simplex);
  // ompute the simplex centroid
  get_centroid(&simplex, &centroid);
  iter_count++;

  // continue minimization until stop conditions are met
  while (continue_minimization(&simplex, eval_count, iter_count, optimset)) {
    int shrink = 0;

    if (optimset->verbose) {
      printf("Iteration %04d     ", iter_count);
    }
    update_point(&simplex, &centroid, RHO, &point_r);
    cost_function(n, &point_r, args);
    eval_count++;
    if (point_r.fx < simplex.p[0].fx) {
      update_point(&simplex, &centroid, RHO * CHI, &point_e);
      cost_function(n, &point_e, args);
      eval_count++;
      if (point_e.fx < point_r.fx) {
        // expand
        if (optimset->verbose) {
          printf("expand          ");
        }
        copy_point(n, &point_e, simplex.p + n);
      } else {
        // reflect
        if (optimset->verbose) {
          printf("reflect         ");
        }
        copy_point(n, &point_r, simplex.p + n);
      }
    } else {
      if (point_r.fx < simplex.p[n - 1].fx) {
        // reflect
        if (optimset->verbose) {
          printf("reflect         ");
        }
        copy_point(n, &point_r, simplex.p + n);
      } else {
        if (point_r.fx < simplex.p[n].fx) {
          update_point(&simplex, &centroid, RHO * GAMMA, &point_c);
          cost_function(n, &point_c, args);
          eval_count++;
          if (point_c.fx <= point_r.fx) {
            // contract outside
            if (optimset->verbose) {
              printf("contract out    ");
            }
            copy_point(n, &point_c, simplex.p + n);
          } else {
            // shrink
            if (optimset->verbose) {
              printf("shrink         ");
            }
            shrink = 1;
          }
        } else {
          update_point(&simplex, &centroid, -GAMMA, &point_c);
          cost_function(n, &point_c, args);
          eval_count++;
          if (point_c.fx <= simplex.p[n].fx) {
            // contract inside
            if (optimset->verbose) {
              printf("contract in     ");
            }
            copy_point(n, &point_c, simplex.p + n);
          } else {
            // shrink
            if (optimset->verbose) {
              printf("shrink          ");
            }
            shrink = 1;
          }
        }
      }
    }
    if (shrink) {
      for (int i = 1; i < n + 1; i++) {
        for (int j = 0; j < n; j++) {
          simplex.p[i].x[j] = simplex.p[0].x[j] +
                              SIGMA * (simplex.p[i].x[j] - simplex.p[0].x[j]);
        }
        cost_function(n, simplex.p + i, args);
        eval_count++;
      }
      simplex_sort(&simplex);
    } else {
      for (int i = n - 1; i >= 0 && simplex.p[i + 1].fx < simplex.p[i].fx; i--) {
        swap_points(simplex.p + (i + 1), simplex.p + i);
      }
    }
    get_centroid(&simplex, &centroid);
    iter_count++;
    if (optimset->verbose) {
      // print current minimum
      printf("[ ");
      for (int i = 0; i < n; i++) {
        printf("%.3e ", simplex.p[0].x[i]);
      }
      printf("]    %.3e \n", simplex.p[0].fx);
    }
  }

  // save solution in output argument
  solution->x = malloc((size_t)n * sizeof(double));
  copy_point(n, simplex.p + 0, solution);

  // free memory
  free(centroid.x);
  free(point_r.x);
  free(point_e.x);
  free(point_c.x);
  for (int i = 0; i < n + 1; i++) {
    free(simplex.p[i].x);
  }
  free(simplex.p);
}

//-----------------------------------------------------------------------------
// Simplex sorting
//-----------------------------------------------------------------------------

int compare(const void *arg1, const void *arg2) {
  const double fx1 = ((const point *)arg1)->fx;
  const double fx2 = ((const point *)arg2)->fx;
  return (fx1 > fx2) - (fx1 < fx2);
}

void simplex_sort(simplex_t *simplex) {
  qsort((void *)(simplex->p), (size_t)simplex->n + 1, sizeof(point), compare);
}

//-----------------------------------------------------------------------------
// Get centroid (average position) of simplex
//-----------------------------------------------------------------------------

void get_centroid(const simplex_t *simplex, point *centroid) {
  for (int j = 0; j < simplex->n; j++) {
    centroid->x[j] = 0;
    for (int i = 0; i < simplex->n; i++) {
      centroid->x[j] += simplex->p[i].x[j];
    }
    centroid->x[j] /= simplex->n;
  }
}

//-----------------------------------------------------------------------------
// Asses if simplex satisfies the minimization requirements
//-----------------------------------------------------------------------------

int continue_minimization(const simplex_t *simplex, int eval_count,
                          int iter_count, const optimset_t *optimset) {
    // stop if #evals or #iters are greater than the max allowed
  if (eval_count > optimset->max_eval) {
    printf("Too many evaluations!\n");
    return 0;
  } else  if (iter_count > optimset->max_iter) {
    printf("Too many iterations!\n");
    return 0;
  }
  // check fx tolerance condition on fx - input simplex is assumed to be sorted
  const int n = simplex->n;
  const double condf = simplex->p[n].fx - simplex->p[0].fx;
  // check fx tolerance condition on x
  double condx = -1.0;
  for (int i = 1; i < n + 1; i++) {
    for (int j = 0; j < n; j++) {
      const double temp = fabs(simplex->p[0].x[j] - simplex->p[i].x[j]);
      if (condx < temp) {
        condx = temp;
      }
    }
  }
  // continue if both tolx or tolf condition is not met
  return condx > optimset->tolx || condf > optimset->tolf;
}

//-----------------------------------------------------------------------------
// Update current point
//-----------------------------------------------------------------------------

void update_point(const simplex_t *simplex, const point *centroid,
                  double lambda, point *p) {
  const int n = simplex->n;
  for (int j = 0; j < n; j++) {
    p->x[j] = (1.0 + lambda) * centroid->x[j] - lambda * simplex->p[n].x[j];
  }
}
