#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
        .lower = strtold(argv[7], NULL),
        .upper = strtold(argv[8], NULL),
        .step_mode = single
    };
    CHECK(conf.places >= 3 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 100);
    CHECK(conf.m >= 1 && conf.m <= 10000);
    CHECK(conf.delta >= 0.0L && conf.delta <= 1.0L);
    CHECK(conf.k_max >= 1 && conf.k_max <= 100000);
    CHECK(conf.upper >= conf.lower);
    return conf;
}

void find_best (spiral *s, config c) {
    for (int i = 0; i < c.m; i++) {
        if (s->p[i]->f < s->best->f) {
            s->best = s->p[i];
        }
    }
}

point *get_point (spiral *s, model *m, config c) {
    point *p = malloc(sizeof (point));          CHECK(p);
    p->x = malloc((size_t)c.n * sizeof (real)); CHECK(p->x);
    for (int k = 0; k < c.n; k++) {
        p->x[k] = (c.upper - c.lower) * (real)rand() / (real)RAND_MAX + c.lower;
    }
    cost(c.n, p, m);
    s->evaluations++;
    return p;
}

spiral *get_spiral (model *m, config c) {
    srand((unsigned int)time(NULL));
    spiral *s =  malloc(sizeof(spiral));
    s->k = s->evaluations = 0;
    s->p = malloc((size_t)c.m * sizeof (point *));    CHECK(s->p);
    for (int i = 0; i < c.m; i++) {
        s->p[i] = get_point(s, m, c);
    }
    s->update = get_point(s, m, c);
    s->best = s->p[0];
    find_best(s, c);
    s->centre = s->best;
    s->looping = false;
    s->restart = true;
    return s;
}

bool soa (spiral *s, model *m, config c) {
	real r = powl(0.1L / c.k_max, 1.0L / c.k_max);
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->restart) {
        s->restart = false;
    	s->k = 0;
        for (int i = 0; i < c.m; i++) {
            if (s->p[i] != s->centre) {
        		for (int k = 0; k < c.n; k++) {
        			s->p[i]->x[k] = (c.upper - c.lower) * (real)rand() / (real)RAND_MAX + c.lower;
        		}
            }
            cost(c.n, s->p[i], m);
            s->evaluations++;
        }
        find_best(s, c);
        if (s->best->f < s->centre->f) {
            s->centre = s->best;
            s->restart = true;
        }
        while (s->k < c.k_max) {
            for (int i = 0; i < c.m; i++) {
                if (s->p[i] != s->centre) {
                    for (int k = 0; k < c.n; k++) {
                        s->update->x[k] = s->centre->x[k] +
                            r * (k ? s->p[i]->x[k - 1] - s->centre->x[k - 1] : s->centre->x[c.n - 1] - s->p[i]->x[c.n - 1]);
                    }
                    bool oor = false;
                    for (int k = 0; k < c.n; k++) {
                        if (s->update->x[k] > c.upper || s->update->x[k] < c.lower) {
                            oor = true;
                            break;
                        }
                    }
                    for (int k = 0; k < c.n; k++) {
                        s->p[i]->x[k] = oor ? (c.upper - c.lower) * (real)rand() / (real)RAND_MAX + c.lower : s->update->x[k];
                    }
                }
                cost(c.n, s->p[i], m);
                s->evaluations++;
            }
            find_best(s, c);
            if (s->best->f < s->centre->f) {
                s->centre = s->best;
                s->restart = true;
            }
            if (++s->k % 10 == 0) {
                printf("  %05d %06d  [ ", s->k, s->evaluations);
                for (int k = 0; k < c.n; k++) {
                    printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, s->centre->x[k]);
                }
                printf(c.fmt ? "]  % .*Le\n" : "]  % .*Lf\n", c.places, s->centre->f);
            }
            if (c.step_mode) return true;
            resume: ;
        }
    }
    return s->looping = false;
}
