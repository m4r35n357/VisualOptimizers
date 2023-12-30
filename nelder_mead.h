
#include "model.h"

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

/*
 * Optimizer settings
 */
typedef struct Optimset {
    real x_tolerance;    // tolerance on the simplex solutions coordinates
    real f_tolerance;    // tolerance on the function value
    int max_iterations;   // maximum number of allowed iterations
    int max_evaluations;   // maximum number of allowed function evaluations
    int debug;    // toggle verbose output during minimization
    int adaptive;  // simplex updates reduced for dimension > 2
    real size;  // size of initial simplex
    int precision;  // significant figures in float/exponential output
    int fmt;  // fixed or exponential floating point format
} optimset;

/*
 * The "simplex" containing an array of n + 1 points each of dimension n
 */
typedef struct Simplex {
    point *p, *reflect, *expand, *contract, *centre;
    int n, iterations, evaluations;
} simplex;

/*
 * "Simplex" or "Amoeba" optimizer
 */
void nelder_mead (simplex *, point *, const model *, const optimset *);

/*
 * Utility functions
 */
point *get_point (int);

simplex *get_simplex (int, real, const point *);

real distance (int, const point *, const point *);

int compare (const void *, const void *);

void sort (simplex *);

void get_centroid (const simplex *, point *);

void project (point *, simplex *, const model *, const point *, real, const point *, point *);

void copy_point (int, const point *, point *);

void print_point (int, const point *, int, int);
