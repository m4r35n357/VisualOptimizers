#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "whale.h"

real cost (real *position, int n) {
    real PI = acos(-1.0L);
    real a = 10.0L;
    real value = a * n;
    for (int j = 0; j < n; j++) {
        real xi = position[j];
        value += xi * xi - a * cosl(2.0L * PI * xi);
    }
    return value;
}

int main(int argc, char *argv[]) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 6);

    int dim = (int)strtol(argv[1], NULL, BASE);         CHECK(dim >= 1 && dim <= 100);
    int num_whales = (int)strtol(argv[2], NULL, BASE);  CHECK(num_whales >= 1 && num_whales <= 10000);
    int max_iter = (int)strtol(argv[3], NULL, BASE);    CHECK(max_iter >= 1 && max_iter <= 10000);
    real minx = strtold(argv[4], NULL);
    real maxx = strtold(argv[5], NULL);

    point *solution = create_whale(dim, minx, maxx);
    woa(solution, max_iter, num_whales, dim, minx, maxx);
    printf("Best solution found:  ");
    for (int k = 0; k < dim; k++) {
        printf("% .6Lf ", solution->x[k]);
    }
    printf("  value = % .6Lf\n\n", cost(solution->x, dim));

    return 0;
}
