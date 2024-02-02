#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
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

point *get_point (int dim, real min_x, real max_x, model *m) {
    point *p = malloc(sizeof(point));
    p->x = malloc((size_t)dim * sizeof(real));
    for (int j = 0; j < dim; j++) {
        p->x[j] = (max_x - min_x) * randreal() + min_x;
    }
    cost(dim, p, m);
    return p;
}

static void find_best (spiral *s, config c) {
	s->i_b = s->points[0];
    for (int i = 1; i < c.m; i++) {
        if (s->points[i]->f < s->i_b->f) {
        	s->i_b = s->points[i];
        }
    }
}

spiral *get_spiral (real min_x, real max_x, model *m, config c) {
    spiral *s =  malloc(sizeof(spiral));
    s->R = malloc(sizeof(real[c.m][c.m]));
    for (int i = 0; i < c.m; i++) {
        for (int k = 0; k < c.m; k++) {
        	if (i == k + 1) {
        		s->R[i][k] = 1.0L;
        	} else {
                s->R[i][k] = 0.0L;
        	}
        }
    }
    s->R[0][c.m - 1] = -1.0L;
    s->k = s->evaluations = 0;
    s->points = malloc((size_t)c.m * sizeof(point *));
    for (int i = 0; i < c.m; i++) {
        s->points[i] = get_point(c.n, min_x, max_x, m);
        s->evaluations++;
    }
    s->x_star = get_point(c.n, min_x, max_x, m);
    s->evaluations++;
    find_best(s, c);
    s->x_star = s->i_b;
    s->looping = false;
    return s;
}

bool soa (spiral *s, point *solution, real min_x, real max_x, model *m, config c) {
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->k < c.k_max) {
    	real r = powl(c.delta, -c.k_max);
        for (int i = 0; i < c.m; i++) {
            for (int k = 0; k < c.m; k++) {
            	for (int j = 0; j < c.n; j++) {
            		s->points[k]->x[j] = s->x_star->x[j] + r * s->R[i][k] * (s->points[k]->x[j] - s->x_star->x[j]);
            	}
            }
            cost(c.n, s->points[i], m);
        }
        cost(c.n, s->x_star, m);
        find_best(s, c);
		if (s->i_b->f < s->x_star->f) s->x_star = s->i_b;
        s->k++;
        printf(" %05d %06d  [ ", s->k, s->evaluations);
        for (int j = 0; j < c.n; j++) {
            printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, s->x_star->x[j]);
        }
        printf(c.fmt ? "]  % .*Le\n" : "]  % .*Lf\n", c.places, s->x_star->f);
        if (c.step_mode) return true;
        resume: ;
    }
    for (int i = 0; i < c.m; i++) {
        for (int j = 0; j < c.n; j++) {
            solution->x[j] = s->x_star->x[j];
        }
        solution->f = s->x_star->f;
    }
    return s->looping = false;
}
