#include <stdio.h>
#include <stdlib.h>
#include "base.h"

/*
 * Random number utilities
 */
void randomize () {
    unsigned int seed = 0;
    FILE *devrnd = fopen("/dev/urandom","r");  CHECK(devrnd);
    fread(&seed, 4, 1, devrnd);
    int opened = fclose(devrnd);  CHECK(!opened);
    srand(seed);
}

real rand_range (real lower, real upper) {
    return (upper - lower) * (real)rand() / (real)RAND_MAX + lower;
}

/*
 * Point utilities
 */
point *get_point (int n) {
    point *p = malloc(sizeof (point));        CHECK(p);
    p->x = malloc((size_t)n * sizeof (real)); CHECK(p->x);
    return p;
}

void set_random_coordinates (point *p, int n, real lower, real upper) {
    for (int j = 0; j < n; j++) {
        p->x[j] = (upper - lower) * (real)rand() / (real)RAND_MAX + lower;
    }
}

void copy_point (int n, const point *src, point *dst) {
    for (int i = 0; i < n; i++) {
        dst->x[i] = src->x[i];
    }
    dst->f = src->f;
}

void print_result (int n, const point *p, int places, int fmt) {
    fprintf(stderr, "%s[%s ", GRY, NRM);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, fmt ? "% .*Le " : "% .*Lf ", places, p->x[i]);
    }
    fprintf(stderr, fmt ? "%s]%s % .*Le\n" : "%s]%s % .*Lf\n", GRY, NRM, places, p->f);
}
