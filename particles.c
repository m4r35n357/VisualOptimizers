#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "particles.h"

config get_config (char **argv, bool single) {
    randomize();
    config conf = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .m = (int)strtol(argv[4], NULL, BASE),
        .max_iterations = (int)strtol(argv[5], NULL, BASE),
        .mode = (int)strtol(argv[6], NULL, BASE),
        .lower = strtold(argv[7], NULL),
        .upper = strtold(argv[8], NULL),
        .step_mode = single
    };
    CHECK(conf.places >= 1 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 64);
    CHECK(conf.m >= 1 && conf.m <= 100000);
    CHECK(conf.max_iterations >= 1 && conf.max_iterations <= 1000);
    CHECK(conf.mode == 0 || conf.mode == 1);
    CHECK(conf.upper > conf.lower);
    return conf;
}

static point **set_initial_population (population *p, model *m, config c) {
    p->points = malloc((size_t)c.m * sizeof (point *)); CHECK(p->points);
    for (int i = 0; i < c.m; i++) {
        p->points[i] = get_point(c.n); CHECK(p->points[i]);
        set_random_coordinates(p->points[i], c.n, c.lower, c.upper);
        cost(c.n, p->points[i], m);
        p->evaluations++;
    }
    p->best = p->points[0];
    for (int i = 1; i < c.m; i++) {
        if (p->points[i]->f < p->best->f) p->best = p->points[i];
    }
    return p->points;
}

static void print_progress (int iterations, int evaluations, point *best, config c) {
    printf("  %5d %6d  [ ", iterations, evaluations);
    for (int k = 0; k < c.n; k++) {
        printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, best->x[k]);
    }
    printf(c.fmt ? "] % .*Le\n" : "] % .*Lf\n", c.places, best->f);
}

population *get_spiral (model *m, config c) {
    population *s =  malloc(sizeof(population));
    s->update = get_point(c.n);
    s->k_star = s->k = s->evaluations = 0;
    s->points = set_initial_population(s, m, c);
    s->x_star = s->best;
    s->looping = s->shrinking = false;
    s->rd = powl(SHRINK_FACTOR, 1.0L / c.max_iterations);
    s->rc = powl(OMEGA, 0.5L / c.n);
    return s;
}

bool soa (population *s, model *m, config c) {
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->k < c.max_iterations) {
        if (c.mode) s->shrinking = s->k >= s->k_star + 2 * c.n;
        for (int i = 0; i < c.m; i++) {
            if (s->points[i] != s->x_star) {
                bool oor = false;
                for (int k = 0; k < c.n; k++) {
                    real rot = !k ? s->x_star->x[c.n - 1] - s->points[i]->x[c.n - 1] : s->points[i]->x[k - 1] - s->x_star->x[k - 1];
                    s->update->x[k] = s->x_star->x[k] + (c.mode ? (s->shrinking ? s->rc : 1.0L) : s->rd) * rot;
                    if (s->update->x[k] > c.upper || s->update->x[k] < c.lower) {
                        oor = true;
                        break;
                    }
                }
                for (int k = 0; k < c.n; k++) {
                    s->points[i]->x[k] = oor ? rand_range(c.lower, c.upper) : s->update->x[k];
                }
                cost(c.n, s->points[i], m);
                s->evaluations++;
                if (s->points[i]->f < s->best->f) s->best = s->points[i];
            }
        }
        s->updated = false;
        if (s->best->f < s->x_star->f) {
            s->updated = true;
            s->x_star = s->best;
            s->k_star = s->k + 1;
            print_progress(s->k_star, s->evaluations, s->x_star, c);
        }
        s->k++;
        if (c.step_mode) return true;
        resume: ;
    }
    return s->looping = false;
}

population *get_box (model *m, config c) {
    population *b =  malloc(sizeof(population));
    b->upper = malloc((size_t)c.n * sizeof (real)); CHECK(b->upper);
    b->lower = malloc((size_t)c.n * sizeof (real)); CHECK(b->lower);
    for (int k = 0; k < c.n; k++) {
        b->upper[k] = c.upper;
        b->lower[k] = c.lower;
    }
    b->iterations = b->evaluations = 0;
    b->points = set_initial_population(b, m, c);
    b->looping = false;
    b->lambda = powl(SHRINK_FACTOR, 1.0L / c.max_iterations);
    return b;
}

bool coa (population *b, model *m, config c) {
    if (c.step_mode && b->looping) goto resume; else b->looping = true;
    while (b->iterations < c.max_iterations) {
        for (int k = 0; k < c.n; k++) {
            real side = 0.5L * b->lambda * (b->upper[k] - b->lower[k]);
            real upper = b->best->x[k] + side;
            real lower = b->best->x[k] - side;
            real lower_limit = c.mode ? b->lower[k] : c.lower;
            real upper_limit = c.mode ? b->upper[k] : c.upper;
            if (lower < lower_limit) {
                upper += lower_limit - lower;
                lower = lower_limit;
            } else if (upper > upper_limit) {
                lower += upper_limit - upper;
                upper = upper_limit;
            }
            b->upper[k] = upper;
            b->lower[k] = lower;
        }
        for (int i = 0; i < c.m; i++) {
            if (b->points[i] != b->best) {
                for (int k = 0; k < c.n; k++) {
                    b->points[i]->x[k] = rand_range(b->lower[k], b->upper[k]);
                }
                cost(c.n, b->points[i], m);
                b->evaluations++;
                if (b->points[i]->f < b->best->f) {
                    b->best = b->points[i];
                    print_progress(b->iterations + 1, b->evaluations, b->best, c);
                }
            }
        }
        b->iterations++;
        if (c.step_mode) return true;
        resume: ;
    }
    return b->looping = false;
}
