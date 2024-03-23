#include <stdio.h>
#include <stdlib.h>
#include "spiral.h"
#include "simplex.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 10);

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
    print_result(c.n, best, c.places, c.fmt);

    if (c.nelder_mead) {
        // Nelder-Mead options
        optimset opt = {
            .places = c.places,
            .fmt = c.fmt,
            .n = c.n,
            .tolerance = 1.0e-6L,
            .max_iterations = 1000000,
            .size = (c.upper - c.lower) / 1000.0L,
            .adaptive = c.n >= 8,
            .step_mode = false
        };

        simplex *si = nm_simplex(opt.n, opt.size, best, opt.adaptive);
        for (int i = 0; i < opt.n + 1; i++) {  // initial cost at simplex vertices
            cost(opt.n, si->p + i, m);
            si->evaluations++;
        }
        sort(si);

        nelder_mead(si, m, &opt);

        // print Nelder-Mead solution
        fprintf(stderr, "   %4d   %4d  ", si->iterations, si->evaluations);
        print_result(opt.n, si->p, opt.places, opt.fmt);

        // print best overall solution
        fprintf(stderr, "        %6d  ", sp1->evaluations + sp2->evaluations + si->evaluations);
        print_result(opt.n, si->p[0].f < best->f ? si->p : best, opt.places, opt.fmt);
    }

    return 0;
}
