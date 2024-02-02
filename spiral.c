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
        .step_mode = single
    };
    CHECK(conf.places >= 3 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 100);
    CHECK(conf.m >= 1 && conf.m <= 10000);
    CHECK(conf.k_max >= 1 && conf.k_max <= 10000);
    return conf;
}

int randint (int n) {
    return (int)((real)rand() / ((real)RAND_MAX + 1) * n);
}

real randreal () {
    return (real)rand() / (real)RAND_MAX;
}

whale *get_point (int dim, real min_x, real max_x, model *m) {
    point *p = malloc(sizeof(point));
    p->x = malloc((size_t)dim * sizeof(real));
    for (int j = 0; j < dim; j++) {
        p->x[j] = (max_x - min_x) * randreal() + min_x;
    }
    cost(dim, p, m);
    return p;
}

spiral *get_spiral (real min_x, real max_x, model *m, config c) {
    srand((unsigned int)time(NULL));
    spiral *s =  malloc(sizeof(spiral));
    s->iterations = s->evaluations = 0;
    s->points = malloc((size_t)c.m * sizeof(point *));
    for (int i = 0; i < c.m; i++) {
        s->points[i] = get_point(c.n, min_x, max_x, m);
        s->evaluations++;
    }
    s->evaluations++;
    for (int i = 0; i < c.m; i++) {
        if (s->points[i]->f < s->x_star->f) {
            for (int j = 0; j < c.n; j++) {
                s->x_star->x[j] = s->points[i]->x[j];
            }
            s->x_star->f = s->points[i]->f;
        }
    }
    s->looping = false;
    return s;
}

bool soa (spiral *s, point *solution, real min_x, real max_x, model *m, config c) {
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->iterations < c.k_max) {
    	/*
        for (int i = 0; i < o.whales; i++) {
            for (int j = 0; j < o.dim; j++) {
                p->whales[i]->x[j] = fmaxl(p->whales[i]->x[j], min_x);
                p->whales[i]->x[j] = fminl(p->whales[i]->x[j], max_x);
            }
            cost(o.dim, p->whales[i], m);
            p->evaluations++;
            if (p->whales[i]->f < p->Xp->f) {
                for (int j = 0; j < o.dim; j++) {
                    p->Xp->x[j] = p->whales[i]->x[j];
                }
                p->Xp->f = p->whales[i]->f;
            }
        }
        */
        s->iterations++;
        printf(" %05d %06d  [ ", s->iterations, s->evaluations);
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
