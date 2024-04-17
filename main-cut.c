#include <stdio.h>
#include <stdlib.h>
#include "cut.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    config c = get_config(argv, false);

    // model parameters
    model *m = model_init();

    box *b = get_box(m, c);

    // run optimization
    coa(b, m, c);

    // print solution
    fprintf(stderr, "  %5d %6d  ", b->iterations, b->evaluations);
    print_result(c.n, b->best, c.places, c.fmt);

    return 0;
}
