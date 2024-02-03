#include <stdio.h>
#include <stdlib.h>
#include "spiral.h"

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    config c = get_config(argv, false);
    real min = strtold(argv[6], NULL);
    real max = strtold(argv[7], NULL);

    // model parameters
    model *m = model_init();

    spiral *s = get_spiral(min, max, m, c);
    for (int k = 0; k < c.n; k++) {
        for (int l = 0; l < c.n; l++) {
            printf("% .1Lf ", s->R[k][l]);
        }
        printf("\n");
    }
    soa(s, m, c);

    fprintf(stderr, "    %s[%s", GRY, NRM);
    for (int k = 0; k < c.n; k++) {
        fprintf(stderr, c.fmt ? "% .*Le " : "% .*Lf ", c.places, s->x_star->x[k]);
    }
    fprintf(stderr, "%s]%s", GRY, NRM);
    fprintf(stderr, "  % .*Lf\n", c.places, s->x_star->f);

    return 0;
}
