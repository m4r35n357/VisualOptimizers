#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "spiral.h"

config get_config (char **argv, bool single) {
    config conf = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .m = (int)strtol(argv[4], NULL, BASE),
        .k_max = (int)strtol(argv[5], NULL, BASE),
        .delta = strtold(argv[6], NULL),
        .step_mode = single
    };
    CHECK(conf.places >= 3 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 100);
    CHECK(conf.m >= 1 && conf.m <= 10000);
    CHECK(conf.delta >= 1.0e-36L && conf.delta <= 1.0e-3L);
    CHECK(conf.k_max >= 1 && conf.k_max <= 100000);
    return conf;
}

int randint (int n) {
    return (int)((real)rand() / ((real)RAND_MAX + 1) * n);
}

real randreal () {
    return (real)rand() / (real)RAND_MAX;
}

static void find_best (spiral *s, config c) {
    for (int i = 0; i < c.m; i++) {
        if (s->points[i]->f < s->i_b->f) {
            s->i_b = s->points[i];
        }
    }
}

spiral *get_spiral (real min_x, real max_x, model *m, config c) {
    spiral *s =  malloc(sizeof(spiral));
    s->k = s->evaluations = 0;
    s->points = malloc((size_t)c.m * sizeof (point *));    CHECK(s->points);
    for (int i = 0; i < c.m; i++) {
        s->points[i] = malloc(sizeof (point));    CHECK(s->points[i]);
        s->points[i]->x = malloc((size_t)c.n * sizeof (real));    CHECK(s->points[i]->x);
        for (int k = 0; k < c.n; k++) {
            s->points[i]->x[k] = (max_x - min_x) * randreal() + min_x;
        }
        cost(c.n, s->points[i], m);
        s->evaluations++;
    }
    s->new_point = malloc(sizeof (point));    CHECK(s->new_point);
    s->new_point->x = malloc((size_t)c.n * sizeof (real));    CHECK(s->new_point->x);
    for (int k = 0; k < c.n; k++) {
        s->new_point->x[k] = (max_x - min_x) * randreal() + min_x;
    }
    cost(c.n, s->new_point, m);
    s->evaluations++;
    s->i_b = s->points[0];
    find_best(s, c);
    s->x_star = s->i_b;
    s->looping = false;
    return s;
}

bool soa (spiral *s, model *m, config c) {
    //real r = powl(c.delta, 1.0L / c.k_max);
	real r = 0.9L;
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->k < c.k_max) {
        for (int i = 0; i < c.m; i++) {
            for (int k = 0; k < c.n; k++) {
                s->new_point->x[k] = s->x_star->x[k] +
                    r * (k ? (s->points[i]->x[k - 1] - s->x_star->x[k - 1]) : - (s->points[i]->x[c.n - 1] - s->x_star->x[c.n - 1]));
            }
            for (int k = 0; k < c.n; k++) {
            	s->points[i]->x[k] = s->new_point->x[k];
            }
            cost(c.n, s->points[i], m);
            s->evaluations++;
        }
        find_best(s, c);
        if (s->i_b->f < s->x_star->f) s->x_star = s->i_b;
        s->k++;
        printf(" %05d %06d  [ ", s->k, s->evaluations);
        for (int k = 0; k < c.n; k++) {
            printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, s->x_star->x[k]);
        }
        printf(c.fmt ? "]  % .*Le\n" : "]  % .*Lf\n", c.places, s->x_star->f);
        if (c.step_mode) return true;
        resume: ;
    }
    return s->looping = false;
}