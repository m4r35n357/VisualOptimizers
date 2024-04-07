#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "spiral.h"

config get_config (char **argv, bool single) {
    unsigned int seed = 0;
    FILE *devrnd = fopen("/dev/urandom","r");  CHECK(devrnd);
    fread(&seed, 4, 1, devrnd);
    int opened = fclose(devrnd);  CHECK(!opened);
    srand(seed);
    config conf = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .m = (int)strtol(argv[4], NULL, BASE),
        .k_max = (int)strtol(argv[5], NULL, BASE),
        .convergence = (int)strtol(argv[6], NULL, BASE),
        .lower = strtold(argv[7], NULL),
        .upper = strtold(argv[8], NULL),
        .step_mode = single
    };
    CHECK(conf.places >= 1 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 64);
    CHECK(conf.m >= 1 && conf.m <= 100000);
    CHECK(conf.k_max >= 1 && conf.k_max <= 1000);
    CHECK(conf.convergence == 0 || conf.convergence == 1);
    CHECK(conf.upper > conf.lower);
    return conf;
}

static real rand_range (real lower, real upper) {
    return (upper - lower) * (real)rand() / (real)RAND_MAX + lower;
}

point *get_spiral_point (spiral *s, model *m, config c) {
    point *p = malloc(sizeof (point));          CHECK(p);
    p->x = malloc((size_t)c.n * sizeof (real)); CHECK(p->x);
    for (int k = 0; k < c.n; k++) {
        p->x[k] = rand_range(c.lower, c.upper);
    }
    cost(c.n, p, m);
    s->evaluations++;
    return p;
}

spiral *get_spiral (model *m, config c) {
    spiral *s =  malloc(sizeof(spiral));
    s->k_star = s->k = s->evaluations = 0;
    s->p = malloc((size_t)c.m * sizeof (point *));    CHECK(s->p);
    for (int i = 0; i < c.m; i++) {
        s->p[i] = get_spiral_point(s, m, c);
    }
    s->update = get_spiral_point(s, m, c);
    s->best = s->p[0];
    for (int i = 1; i < c.m; i++) {
        if (s->p[i]->f < s->best->f) s->best = s->p[i];
    }
    s->x_star = s->best;
    s->looping = s->dual_mode = false;
    s->r = powl(0.1L / c.k_max, 1.0L / c.k_max);
    return s;
}

bool soa (spiral *s, model *m, config c) {
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->k < c.k_max) {
        if (c.convergence) s->r = (s->k >= s->k_star + 2 * c.n) ? powl(0.1L, 0.5L / c.n) : 1.0L;
        for (int i = 0; i < c.m; i++) {
            if (s->p[i] != s->x_star) {
                bool oor = false;
                for (int k = 0; k < c.n; k++) {
                    real rot_k;
                    if (s->dual_mode) {
                        rot_k = k == c.n - 1 ? s->x_star->x[0] - s->p[i]->x[0] : s->p[i]->x[k + 1] - s->x_star->x[k + 1];
                    } else {
                        rot_k = k == 0 ? s->x_star->x[c.n - 1] - s->p[i]->x[c.n - 1] : s->p[i]->x[k - 1] - s->x_star->x[k - 1];
                    }
                    s->update->x[k] = s->x_star->x[k] + s->r * rot_k;
                    if (s->update->x[k] > c.upper || s->update->x[k] < c.lower) {
                        oor = true;
                        break;
                    }
                }
                for (int k = 0; k < c.n; k++) {
                    s->p[i]->x[k] = oor ? rand_range(c.lower, c.upper) : s->update->x[k];
                }
                cost(c.n, s->p[i], m);
                s->evaluations++;
                if (s->p[i]->f < s->best->f) s->best = s->p[i];
            }
        }
        if (s->best->f < s->x_star->f) {
            s->x_star = s->best;
            s->k_star = s->k + 1;
            printf("  %5d %6d  [ ", s->k + 1, s->evaluations);
            for (int k = 0; k < c.n; k++) {
                printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, s->x_star->x[k]);
            }
            printf(c.fmt ? "] % .*Le\n" : "] % .*Lf\n", c.places, s->x_star->f);
        }
        s->k++;
        if (c.step_mode) return true;
        resume: ;
    }
    return s->looping = false;
}
