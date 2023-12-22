
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nelder_mead.h"

simplex *regular (int n, real size, const point *centre) {
    simplex *s = malloc(sizeof (simplex));
    s->n = n;
    s->p = malloc((size_t)(n + 1) * sizeof (point));
    for (int i = 0; i < n + 1; i++) {  // simplex vertices
        s->p[i].x = malloc((size_t)n * sizeof (real));
        for (int j = 0; j < n; j++) {  // coordinates
            s->p[i].x[j] = 0.0L;
        }
    }
    real b = 0.0L;
    for (int i = 0; i < n; i++) {
        real c = sqrtl(1.0L - b);
        s->p[i].x[i] = c;
        real r = - (1.0L / n + b) / c;
        for (int j = i + 1; j < n + 1; j++) {
            s->p[j].x[i] = r;
        }
        b += SQR(r);
    }
    for (int i = 0; i < n + 1; i++) {  // simplex vertices
        for (int j = 0; j < n; j++) {  // coordinates
            s->p[i].x[j] = size * s->p[i].x[j] + centre->x[j];
        }
    }
    return s;
}

 real distance (int n, const point *a, const point *b) {
	real sum = 0.0L;
	for (int j = 0; j < n; j++) {
		sum += SQR(a->x[j] - b->x[j]);
	}
	return sqrtl(sum);
}

/*
 * Main function
 * - start is the initial point (unchanged in output)
 * - n is the dimension of the data
 * - solution is the minimizer
 * - args are the optional arguments of cost_function
 * - opt are the optimisation settings
 */
void nelder_mead (int n, const point *start, point *solution, const model *args, const optimset *opt) {
    real ALPHA = 1.0L;
    real GAMMA = opt->adaptive_scaling ? 1.0L + 2.0L / n : 2.0L;
    real RHO = opt->adaptive_scaling ? 0.75L - 0.5L / n : 0.5L;
    real SIGMA = opt->adaptive_scaling ? 1.0L - 1.0L / n : 0.5L;

    // internal points and labels
    point reflected, expanded, contracted, centre, *best, *worst;

    // allocate memory for internal points
    reflected.x = malloc((size_t)n * sizeof(real));  CHECK(reflected.x);
    expanded.x = malloc((size_t)n * sizeof(real));   CHECK(expanded.x);
    contracted.x = malloc((size_t)n * sizeof(real)); CHECK(contracted.x);
    centre.x = malloc((size_t)n * sizeof(real));     CHECK(centre.x);

    int iter_count = 0;
    int eval_count = 1;  // already done one in main.c!

    // initial simplex has size n + 1 where n is the dimensionality of the data
    simplex *s = regular(n, opt->simplex_scaling, start);
    for (int i = 0; i < n + 1; i++) {  // simplex vertices
        cost(n, s->p + i, args);
        eval_count++;
    }
    sort(s);
    best = s->p;
    worst = s->p + n;
    printf("      %sDiameter %s% .*Le\n", GRY, NRM, opt->diplay_precision, distance(n, best, worst));

    while (processing(s, eval_count, iter_count, opt)) {
        iter_count++;
        if (opt->verbose) printf(" %04d %04d  ", iter_count, eval_count);
        int shrink = 0;
        get_centroid(s, &centre);

        project(&reflected, n, &centre, ALPHA, &centre, worst);
        cost(n, &reflected, args);
        eval_count++;
        if (reflected.f < best->f) {
            project(&expanded, n, &centre, GAMMA, &reflected, &centre);
            cost(n, &expanded, args);
            eval_count++;
            if (expanded.f < reflected.f) {
                if (opt->verbose) printf("expand        ");
                copy_point(n, &expanded, worst);
            } else {
                if (opt->verbose) printf("reflect       ");
                copy_point(n, &reflected, worst);
            }
        } else {
            if (reflected.f < (worst - 1)->f) {
                if (opt->verbose) printf("reflect       ");
                copy_point(n, &reflected, worst);
            } else {
                if (reflected.f < worst->f) {
                    project(&contracted, n, &centre, RHO, &reflected, &centre);
                    cost(n, &contracted, args);
                    eval_count++;
                    if (contracted.f < reflected.f) {
                        if (opt->verbose) printf("contract_out  ");
                        copy_point(n, &contracted, worst);
                    } else {
                        shrink = 1;
                    }
                } else {
                    project(&contracted, n, &centre, RHO, worst, &centre);
                    cost(n, &contracted, args);
                    eval_count++;
                    if (contracted.f <= worst->f) {
                        if (opt->verbose) printf("contract_in   ");
                        copy_point(n, &contracted, worst);
                    } else {
                        shrink = 1;
                    }
                }
            }
        }
        if (shrink) {
            if (opt->verbose) printf("shrink        ");
            for (int i = 1; i < n + 1; i++) {
                project(s->p + i, n, best, SIGMA, s->p + i, best);
                cost(n, s->p + i, args);
                eval_count++;
            }
        }
        sort(s);

        if (opt->verbose) { // print current minimum
            printf("[ ");
            for (int i = 0; i < n; i++) {
                printf("% .*Le ", opt->diplay_precision, best->x[i]);
            }
            printf("]  % .*Le\n", opt->diplay_precision, best->f);
        }
    }

    // save solution in output argument
    copy_point(n, best, solution);

    // free memory
    free(centre.x);
    free(reflected.x);
    free(expanded.x);
    free(contracted.x);
    for (int i = 0; i < n + 1; i++) {
        free(s->p[i].x);
    }
    free(s->p);
    free(s);
}

/*
 * Simplex sorting
 */
int compare (const void *arg1, const void *arg2) {
    const real f1 = ((const point *)arg1)->f;
    const real f2 = ((const point *)arg2)->f;
    return (f1 > f2) - (f1 < f2);
}

void sort (simplex *s) {
    qsort((void *)(s->p), (size_t)s->n + 1, sizeof(point), compare);
}

/*
 * Get centroid (average position) of simplex
 */
void get_centroid (const simplex *s, point *c) {
    for (int j = 0; j < s->n; j++) {
        c->x[j] = 0;
        for (int i = 0; i < s->n; i++) {
            c->x[j] += s->p[i].x[j];
        }
        c->x[j] /= s->n;
    }
}

/*
 * Terminate or continue?
 */
int processing (const simplex *s, int eval_count, int iter_count, const optimset *opt) {
    CHECK(eval_count <= opt->max_eval);
    CHECK(iter_count <= opt->max_iter);
    // check tolerance condition on fx - input simplex is assumed to be sorted
    const int n = s->n;
    const real condf = s->p[n].f - s->p[0].f;
    // check tolerance condition on x
    real condx = -1.0L;
    for (int i = 1; i < n + 1; i++) {
        for (int j = 0; j < n; j++) {
            const real temp = fabsl(s->p[0].x[j] - s->p[i].x[j]);
            if (condx < temp) {
                condx = temp;
            }
        }
    }
    // continue if both tolx or tolf condition is not met
    return condx > opt->tolx || condf > opt->tolf;
}

/*
 * Extend a point from p in the direction of (pa - pb), scaled by factor
 */
void project (const point *new, int n, const point *p, real factor, const point *pa, point *pb) {
    for (int j = 0; j < n; j++) {
        new->x[j] = p->x[j] + factor * (pa->x[j] - pb->x[j]);
    }
}

void copy_point (int n, const point *src, point *dst) {
    memcpy(dst->x, src->x, sizeof(real) * (size_t)n);
    dst->f = src->f;
}

void print_point (int n, const point *p, int dp) {
    printf("[ %s", NRM);
    for (int i = 0; i < n; i++) {
        printf("% .*Le ", dp, p->x[i]);
    }
    printf("%s]%s % .*Le\n", GRY, NRM, dp, p->f);
}
