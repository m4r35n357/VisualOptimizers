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
    p->agents = malloc((size_t)c.m * sizeof (point *)); CHECK(p->agents);
    for (int i = 0; i < c.m; i++) {
        p->agents[i] = get_point(c.n); CHECK(p->agents[i]);
        set_random_coordinates(p->agents[i], c.n, c.lower, c.upper);
        cost(c.n, p->agents[i], m);
        p->evaluations++;
    }
    p->best = p->agents[0];
    for (int i = 1; i < c.m; i++) {
        if (p->agents[i]->f < p->best->f) p->best = p->agents[i];
    }
    return p->agents;
}

static void print_progress (int iterations, int evaluations, point *best, config c) {
    printf("  %5d %6d  [ ", iterations, evaluations);
    for (int k = 0; k < c.n; k++) {
        printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, best->x[k]);
    }
    printf(c.fmt ? "] % .*Le\n" : "] % .*Lf\n", c.places, best->f);
}

population *get_spiral (model *m, config c) {  // step 0
    population *s =  malloc(sizeof(population));
    s->update = get_point(c.n);
    s->k_star = s->k = s->evaluations = 0;
    s->agents = set_initial_population(s, m, c);  // step 1
    s->x_star = s->best;
    s->looping = s->shrinking = false;
    s->rd = powl(SHRINK_FACTOR, 1.0L / c.max_iterations);  // step 2 rule for periodic descent direction mode
    s->rc = powl(OMEGA, 0.5L / c.n);
    return s;
}

bool soa (population *s, model *m, config c) {
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->k < c.max_iterations) {
        if (c.mode) s->shrinking = s->k >= s->k_star + 2 * c.n;  // step 2 rule for convergence mode
        for (int i = 0; i < c.m; i++) {
            if (s->agents[i] != s->x_star) {
                for (int k = 0; k < c.n; k++) {  // step 3 - rotate by pi/2 in all dimensions around centre
                    real rot = !k ? s->x_star->x[c.n - 1] - s->agents[i]->x[c.n - 1] : s->agents[i]->x[k - 1] - s->x_star->x[k - 1];
                    s->update->x[k] = s->x_star->x[k] + (c.mode ? (s->shrinking ? s->rc : 1.0L) : s->rd) * rot;
                }
                for (int k = 0; k < c.n; k++) {  // clip any out of range agents
                    s->agents[i]->x[k] = fmaxl(s->update->x[k], c.lower);
                    s->agents[i]->x[k] = fminl(s->update->x[k], c.upper);
                }
                cost(c.n, s->agents[i], m);
                s->evaluations++;
                if (s->agents[i]->f < s->best->f) s->best = s->agents[i];  // look for new best agent
            }
        }
        s->updated = false;
        if (s->best->f < s->x_star->f) {  // step 4 - new centre ?
            s->x_star = s->best;
            s->k_star = s->k + 1;
            s->updated = true;
        }
        s->k++;  // step 5
        if (s->updated) print_progress(s->k, s->evaluations, s->x_star, c);
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
    b->agents = set_initial_population(b, m, c);
    b->looping = false;
    b->lambda = powl(SHRINK_FACTOR, 1.0L / c.max_iterations);
    return b;
}

bool coa (population *b, model *m, config c) {
    if (c.step_mode && b->looping) goto resume; else b->looping = true;
    while (b->iterations < c.max_iterations) {
        for (int k = 0; k < c.n; k++) {  // shrink the box
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
        for (int i = 0; i < c.m; i++) {  // randomize all agents but the best
            if (b->agents[i] != b->best) {
                for (int k = 0; k < c.n; k++) {
                    b->agents[i]->x[k] = rand_range(b->lower[k], b->upper[k]);
                }
                cost(c.n, b->agents[i], m);
                b->evaluations++;
            }
        }
        b->updated = false;
        for (int i = 0; i < c.m; i++) {  // look for new best agent
            if (b->agents[i]->f < b->best->f) {
                b->best = b->agents[i];
                b->updated = true;
            }
        }
        b->iterations++;
        if (b->updated) print_progress(b->iterations, b->evaluations, b->best, c);
        if (c.step_mode) return true;
        resume: ;
    }
    return b->looping = false;
}
