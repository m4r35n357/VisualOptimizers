#include <stdio.h>
#include <stdlib.h>
#include "spiral.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    config c = get_config(argv, false);

    // model parameters
    model *m = model_init();

    spiral *sp1 = get_spiral(m, c);
    spiral *sp2 = get_spiral(m, c);
    for (int i = 0; i < c.m; i++) {
        for (int k = 0; k < c.n; k++) {
            sp2->p[i]->x[k] = sp1->p[i]->x[k];
        }
    }
    sp2->best = sp1->best;
    sp2->x_star = sp1->x_star;
    sp2->dual_mode = true;

    // run spiral optimizations
    soa(sp1, m, c);
    soa(sp2, m, c);

    // print best spiral solution
    point *best = sp1->x_star->f <= sp2->x_star->f ? sp1->x_star : sp2->x_star;
    fprintf(stderr, "  %5d %6d  ", sp1->k + sp2->k, sp1->evaluations + sp2->evaluations);
    fprintf(stderr, "%s[%s ", GRY, NRM);
    for (int i = 0; i < c.n; i++) {
        fprintf(stderr, c.fmt ? "% .*Le " : "% .*Lf ", c.places, best->x[i]);
    }
    fprintf(stderr, c.fmt ? "%s]%s % .*Le\n" : "%s]%s % .*Lf\n", GRY, NRM, c.places, best->f);

    return 0;
}
