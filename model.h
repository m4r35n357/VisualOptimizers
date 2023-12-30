typedef long double real;
/*
 * "Low-noise" squaring for arguments with no side-effects
 */
#define SQR(x) ((x) * (x))

/*
 * Point is an array of dimension n together with a cost function value
 */
typedef struct Point {
  real *x;
  real f;
} point;

/*
 * Model parameters
 */
typedef struct Parameters parameters;

/*
 * Function to minimize
 */
void cost (int, point *, const parameters *);

typedef void (*cost_function)(int, point *, const parameters *);

/*
 * Cost function interface
 */
typedef struct Model {
  parameters *p;
  cost_function c;
} model;

/*
 * Initialize model
 */
model *get_model (void);
