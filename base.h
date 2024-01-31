#pragma once

#include <stdbool.h>

#define BASE 10

/*
 * Colours
 */
#define GRY "\x1B[1;30m"
#define RED "\x1B[1;31m"
#define GRN "\x1B[1;32m"
#define YLW "\x1B[1;33m"
#define BLU "\x1B[1;34m"
#define MGT "\x1B[0;35m"
#define CYN "\x1B[0;36m"
#define WHT "\x1B[1;37m"
#define NRM "\x1B[0m"

/*
 * Report program arguments in colour
 */
#define PRINT_ARGS(argc, argv) do { \
    fprintf(stderr, "%sargc %s%d%s, argv [ %s", GRY, NRM, (argc), GRY, CYN); \
    for (int i = 0; i < (argc); i++) fprintf(stderr, "%s ", (argv)[i]); \
    fprintf(stderr, "%s]%s\n", GRY, NRM); \
} while (0)

/*
 * Unavoidable "assert", in colour
 */
#define CHECK(x) do { \
    if(!(x)) { \
        fprintf(stderr, "%sFAIL %s%s %s%s%s %s%s:%s%i\n", RED, WHT, #x, GRY, __func__, NRM, __FILE__, GRY, NRM, __LINE__); \
        exit(1); \
    } \
} while (0)

typedef long double real;

/*
 * Point is an array of dimension n together with a cost function value
 */
typedef struct Point {
  real *x;
  real f;
} point, whale;

/*
 * Optional, where known
 */
typedef struct KnownMinima {
    point *min;
    int n_minima;
} minima;
