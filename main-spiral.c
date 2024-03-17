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

    spiral *sp = get_spiral(m, c);

    // run spiral optimization
    soa(sp, m, c);

    // print spiral solution
    fprintf(stderr, "  %5d %6d ", sp->k, sp->evaluations);
    fprintf(stderr, " %s[%s ", GRY, NRM);
    for (int k = 0; k < c.n; k++) {
        fprintf(stderr, c.fmt ? "% .*Le " : "% .*Lf ", c.places, sp->centre->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, c.fmt ? " % .*Le\n" : " % .*Lf\n", c.places, sp->centre->f);

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

        // single simplex . . .
        simplex *si = nm_simplex(opt.n, opt.size, sp->centre, opt.adaptive);
        for (int i = 0; i < si->n + 1; i++) {  // initial cost at simplex vertices
            cost(si->n, si->p + i, m);
            si->evaluations++;
        }
        sort(si);

        // run Nelder-Mead optimization
        nelder_mead(si, m, &opt);

        // print Nelder-Mead solution
        fprintf(stderr, "   %4d   %4d  ", si->iterations, si->evaluations);
        print_result(si->n, si->p, opt.places, opt.fmt);

        // print best solution
        fprintf(stderr, "        %6d  ",sp->evaluations + si->evaluations);
        print_result(si->n, si->p->f < sp->centre->f ? si->p : sp->centre, c.places, c.fmt);
    }

    return 0;
}
