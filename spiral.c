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
	real best = DBL_MAX;
    for (int i = 0; i < c.m; i++) {
        if (s->points[i].f < best) {
        	best = s->points[i].f;
        	s->i_b = s->points + i;
        }
    }
}

spiral *get_spiral (real min_x, real max_x, model *m, config c) {
    spiral *s =  malloc(sizeof(spiral));
    s->points = malloc((size_t)c.m * sizeof (point));    CHECK(s->points);
    for (int i = 0; i < c.m; i++) {
        s->points[i].x = malloc((size_t)c.n * sizeof (real));    CHECK(s->points[i].x);
        for (int j = 0; j < c.n; j++) {
        	s->points[i].x[j] = (max_x - min_x) * randreal() + min_x;
        }
        cost(c.n, s->points + i, m);
    }
    s->R = malloc(sizeof(real[c.m][c.m]));
    for (int k = 0; k < c.n; k++) {
        for (int l = 0; l < c.n; l++) {
    		s->R[k][l] = (k == l + 1) ? 1.0L : 0.0L;
    		printf("% .1Lf ", s->R[k][l]);
        }
        printf("\n");
    }
    s->R[0][c.m - 1] = -1.0L;
    s->k = s->evaluations = 0;
    find_best(s, c);
    s->x_star = s->i_b;
    s->looping = false;
    return s;
}

bool soa (spiral *s, point *solution, model *m, config c) {
	real r = powl(c.delta, 1.0L / c.k_max);
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->k < c.k_max) {
        for (int i = 0; i < c.m; i++) {
            for (int k = 0; k < c.n; k++) {
            	real temp = 0.0L;
            	for (int l = 0; l < c.n; l++) {
            		temp += s->R[k][l] * (s->points[i].x[l] - s->x_star->x[l]);
            	}
            	s->points[i].x[k] = s->x_star->x[k] + r * temp;
            }
            cost(c.n, s->points + i, m);
            s->evaluations++;
        }
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
