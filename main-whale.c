#include <stdio.h>
#include <stdlib.h>
#include "whale.h"
#include "simplex.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // whale options
    options o = get_options(argv, false);

    // model parameters
    model *m = model_init();

    population *p = get_population(m, o);

    // run whale optimization
    woa(p, m, o);

    // print whale solution
    fprintf(stderr, "  %5d %6d ", p->iterations, p->evaluations);
    fprintf(stderr, " %s[%s ", GRY, NRM);
    for (int k = 0; k < o.dim; k++) {
        fprintf(stderr, o.fmt ? "% .*Le " : "% .*Lf ", o.places, p->prey->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, o.fmt ? " % .*Le\n" : " % .*Lf\n", o.places, p->prey->f);

    if (o.nelder_mead) {
        // Nelder-Mead options
        optimset opt = {
            .places = o.places,
            .fmt = o.fmt,
            .n = o.dim,
            .tolerance = 1.0e-6L,
            .max_iterations = 1000000,
            .size = (o.upper - o.lower) / 1000.0L,
            .adaptive = o.dim >= 8,
            .step_mode = false
        };

        // single simplex . . .
        simplex *s = nm_simplex(opt.n, opt.size, p->prey, opt.adaptive);
        for (int i = 0; i < s->n + 1; i++) {  // initial cost at simplex vertices
            cost(s->n, s->p + i, m);
            s->evaluations++;
        }
        sort(s);

        // run Nelder-Mead optimization
        nelder_mead(s, m, &opt);

        // print Nelder-Mead solution
        fprintf(stderr, "   %4d   %4d  ", s->iterations, s->evaluations);
        print_result(s->n, s->p, opt.places, opt.fmt);

        // print best solution
        fprintf(stderr, "        %6d  ",p->evaluations + s->evaluations);
        print_result(s->n, s->p->f < p->prey->f ? s->p : p->prey, o.places, o.fmt);
    }

    return 0;
}
