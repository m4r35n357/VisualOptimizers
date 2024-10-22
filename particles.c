#include <string.h>
#include <math.h>
#include "particles.h"

config get_config (char **argv) {
    randomize();
    config conf = {
        .step_mode = strstr(argv[0], "-gl") ? true : false,
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = strstr(argv[2], "fixed") ? 0 : (strstr(argv[2], "exp") ? 1 : -1),
        .n = (int)strtol(argv[3], NULL, BASE),
        .m = (int)strtol(argv[4], NULL, BASE),
        .max_iterations = (int)strtol(argv[5], NULL, BASE),
        .mode = strstr(argv[6], "unclamped") ? 0 : (strstr(argv[6], "clamped") ? 1 : (strstr(argv[6], "random") ? 2 : -1)),
        .lower = strtold(argv[7], NULL),
        .upper = strtold(argv[8], NULL)
    };
    CHECK(conf.places >= 1 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 64);
    CHECK(conf.m >= 1 && conf.m <= 100000);
    CHECK(conf.max_iterations >= 1 && conf.max_iterations <= 1000);
    CHECK(conf.mode == 0 || conf.mode == 1 || conf.mode == 2);
    CHECK(conf.upper > conf.lower);
    return conf;
}

static point **create_population (population *p, model *m, config *c) {
    p->agents = malloc((size_t)c->m * sizeof (point *)); CHECK(p->agents);
    for (int i = 0; i < c->m; i++) {
        p->agents[i] = get_point(c->n); CHECK(p->agents[i]);
        set_random_coordinates(p->agents[i], c->n, c->lower, c->upper);
        cost(c->n, p->agents[i], m);
        p->evaluations++;
    }
    p->best = p->agents[0];
    for (int i = 1; i < c->m; i++) {
        if (p->agents[i]->f < p->best->f) p->best = p->agents[i];
    }
    return p->agents;
}

static void print_progress (int iterations, int evaluations, point *best, config *c) {
    printf("  %5d %6d  [ ", iterations, evaluations);
    for (int k = 0; k < c->n; k++) {
        printf(c->fmt ? "% .*Le " : "% .*Lf ", c->places, best->x[k]);
    }
    printf(c->fmt ? "] % .*Le\n" : "] % .*Lf\n", c->places, best->f);
}

population *get_box (model *m, config *c) {
    population *b =  malloc(sizeof(population));
    b->upper = malloc((size_t)c->n * sizeof (real)); CHECK(b->upper);
    b->lower = malloc((size_t)c->n * sizeof (real)); CHECK(b->lower);
    for (int k = 0; k < c->n; k++) {
        b->upper[k] = c->upper;
        b->lower[k] = c->lower;
    }
    b->iterations = b->evaluations = 0;
    b->agents = create_population(b, m, c);
    b->looping = false;
    b->lambda = powl(SHRINK_FACTOR, 1.0L / c->max_iterations);
    b->side = 0.5L * (c->upper - c->lower);
    return b;
}

bool coa (population *b, model *m, config *c) {
    if (c->step_mode && b->looping) goto resume; else b->looping = true;
    while (b->iterations < c->max_iterations) {
        if (c->mode != 2) {  // cut algorithm - shrink the box
            b->side *= b->lambda;
            for (int k = 0; k < c->n; k++) {
                real upper = b->best->x[k] + b->side;
                real lower = b->best->x[k] - b->side;
                real lower_limit = c->mode ? b->lower[k] : c->lower;
                real upper_limit = c->mode ? b->upper[k] : c->upper;
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
        }
        for (int i = 0; i < c->m; i++) {  // randomize all agents but the best
            if (b->agents[i] != b->best) {
                for (int k = 0; k < c->n; k++) {
                    b->agents[i]->x[k] = rand_range(b->lower[k], b->upper[k]);
                }
                cost(c->n, b->agents[i], m);
                b->evaluations++;
            }
        }
        b->updated = false;
        for (int i = 0; i < c->m; i++) {  // look for new best agent
            if (b->agents[i]->f < b->best->f) {
                b->best = b->agents[i];
                b->updated = true;
            }
        }
        b->iterations++;
        if (b->updated) print_progress(b->iterations, b->evaluations, b->best, c);
        if (c->step_mode) return true;
        resume: ;
    }
    return b->looping = false;
}
