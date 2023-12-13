
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nelder_mead.h"

//-----------------------------------------------------------------------------
// Main function
// - n is the dimension of the data
// - start is the initial point (unchanged in output)
// - solution is the minimizer
// - args are the optional arguments of cost_function
// - opt are the optimisation settings
//-----------------------------------------------------------------------------
void nelder_mead(int n, const point *start, point *solution, const model *args, const optimset *opt) {
  // internal points
  point reflected;
  point expanded;
  point contracted;
  point centroid;

  // allocate memory for internal points
  reflected.x = malloc((size_t)n * sizeof(double));
  expanded.x = malloc((size_t)n * sizeof(double));
  contracted.x = malloc((size_t)n * sizeof(double));
  centroid.x = malloc((size_t)n * sizeof(double));

  int iter_count = 0;
  int eval_count = 0;

  // initial simplex has size n + 1 where n is the dimensionality pf the data
  simplex s;
  s.n = n;
  s.p = malloc((size_t)(n + 1) * sizeof(point));
  for (int i = 0; i < n + 1; i++) {
    s.p[i].x = malloc((size_t)n * sizeof(double));
    for (int j = 0; j < n; j++) {
      s.p[i].x[j] = (i - 1 == j) ? (start->x[j] != 0.0 ? 1.05 * start->x[j] : 0.00025) : start->x[j];
    }
    cost(n, s.p + i, args);
    eval_count++;
  }
  // sort points in the simplex so that simplex.p[0] is the point having
  // minimum fx and simplex.p[n] is the one having the maximum fx
  sort(&s);
  // compute the simplex centroid
  get_centroid(&s, &centroid);
  iter_count++;

  // continue minimization until stop conditions are met
  while (processing(&s, eval_count, iter_count, opt)) {
    int shrink = 0;

    if (opt->verbose) {
      printf(" %04d %04d  ", iter_count, eval_count);
    }
    project(&s, &centroid, RHO, &reflected);
    cost(n, &reflected, args);
    eval_count++;
    if (reflected.fx < s.p[0].fx) {
      project(&s, &centroid, RHO * CHI, &expanded);
      cost(n, &expanded, args);
      eval_count++;
      if (expanded.fx < reflected.fx) {
        // expand
        if (opt->verbose) {
          printf("expand        ");
        }
        copy_point(n, &expanded, s.p + n);
      } else {
        // reflect
        if (opt->verbose) {
          printf("reflect       ");
        }
        copy_point(n, &reflected, s.p + n);
      }
    } else {
      if (reflected.fx < s.p[n - 1].fx) {
        // reflect
        if (opt->verbose) {
          printf("reflect       ");
        }
        copy_point(n, &reflected, s.p + n);
      } else {
        if (reflected.fx < s.p[n].fx) {
          project(&s, &centroid, RHO * GAMMA, &contracted);
          cost(n, &contracted, args);
          eval_count++;
          if (contracted.fx <= reflected.fx) {
            // contract outside
            if (opt->verbose) {
              printf("contract_out  ");
            }
            copy_point(n, &contracted, s.p + n);
          } else {
            // shrink
            if (opt->verbose) {
              printf("shrink       ");
            }
            shrink = 1;
          }
        } else {
          project(&s, &centroid, -GAMMA, &contracted);
          cost(n, &contracted, args);
          eval_count++;
          if (contracted.fx <= s.p[n].fx) {
            // contract inside
            if (opt->verbose) {
              printf("contract_in   ");
            }
            copy_point(n, &contracted, s.p + n);
          } else {
            // shrink
            if (opt->verbose) {
              printf("shrink        ");
            }
            shrink = 1;
          }
        }
      }
    }
    if (shrink) {
      for (int i = 1; i < n + 1; i++) {
        for (int j = 0; j < n; j++) {
          s.p[i].x[j] = s.p[0].x[j] + SIGMA * (s.p[i].x[j] - s.p[0].x[j]);
        }
        cost(n, s.p + i, args);
        eval_count++;
      }
      sort(&s);
    } else {
      for (int i = n - 1; i >= 0 && s.p[i + 1].fx < s.p[i].fx; i--) {
        swap_points(s.p + (i + 1), s.p + i);
      }
    }
    get_centroid(&s, &centroid);
    iter_count++;
    if (opt->verbose) {
      // print current minimum
      printf("[ ");
      for (int i = 0; i < n; i++) {
        printf("% .9e ", s.p[0].x[i]);
      }
      printf("]  % .6e\n", s.p[0].fx);
    }
  }

  // save solution in output argument
  solution->x = malloc((size_t)n * sizeof(double));
  copy_point(n, s.p + 0, solution);

  // free memory
  free(centroid.x);
  free(reflected.x);
  free(expanded.x);
  free(contracted.x);
  for (int i = 0; i < n + 1; i++) {
    free(s.p[i].x);
  }
  free(s.p);
}

//-----------------------------------------------------------------------------
// Simplex sorting
//-----------------------------------------------------------------------------
int compare(const void *arg1, const void *arg2) {
  const double fx1 = ((const point *)arg1)->fx;
  const double fx2 = ((const point *)arg2)->fx;
  return (fx1 > fx2) - (fx1 < fx2);
}

void sort(simplex *s) {
  qsort((void *)(s->p), (size_t)s->n + 1, sizeof(point), compare);
}

//-----------------------------------------------------------------------------
// Get centroid (average position) of simplex
//-----------------------------------------------------------------------------
void get_centroid(const simplex *s, point *centroid) {
  for (int j = 0; j < s->n; j++) {
    centroid->x[j] = 0;
    for (int i = 0; i < s->n; i++) {
      centroid->x[j] += s->p[i].x[j];
    }
    centroid->x[j] /= s->n;
  }
}
/*
 * Terminate or continue?
 */
int processing(const simplex *s, int eval_count, int iter_count, const optimset *opt) {
    // stop if #evals or #iters are greater than the max allowed
  if (eval_count > opt->max_eval) {
    printf("Too many evaluations!\n");
    return 0;
  } else  if (iter_count > opt->max_iter) {
    printf("Too many iterations!\n");
    return 0;
  }
  // check fx tolerance condition on fx - input simplex is assumed to be sorted
  const int n = s->n;
  const double condf = s->p[n].fx - s->p[0].fx;
  // check fx tolerance condition on x
  double condx = -1.0;
  for (int i = 1; i < n + 1; i++) {
    for (int j = 0; j < n; j++) {
      const double temp = fabs(s->p[0].x[j] - s->p[i].x[j]);
      if (condx < temp) {
        condx = temp;
      }
    }
  }
  // continue if both tolx or tolf condition is not met
  return condx > opt->tolx || condf > opt->tolf;
}

//-----------------------------------------------------------------------------
// Update current point
//-----------------------------------------------------------------------------
void project(const simplex *s, const point *centroid, double lambda, point *p) {
  const int n = s->n;
  for (int j = 0; j < n; j++) {
    p->x[j] = (1.0 + lambda) * centroid->x[j] - lambda * s->p[n].x[j];
  }
}

void swap_points(point *p1, point *p2) {
  point p = *p1;
  *p1 = *p2;
  *p2 = p;
}

void copy_point(int n, const point *src, point *dst) {
  memcpy(dst->x, src->x, sizeof(double) * (size_t)n);
  dst->fx = src->fx;
}

void print_point(int n, const point *p) {
  printf("[ %s", NRM);
  for (int i = 0; i < n; i++) {
    printf("% .9e ", p->x[i]);
  }
  printf("%s]  f%s % .6e\n", GRY, NRM, p->fx);
}

