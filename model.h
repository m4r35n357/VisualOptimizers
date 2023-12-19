typedef long double real;
/*
 * "Low-noise" squaring for arguments with no side-effects
 */
#define SQR(x) ((x) * (x))

/*
 * Model parameters
 */
typedef struct Model model;

/*
 * Point is an array of dimension n together with a cost function value
 */
typedef struct Point {
  real *x;
  real f;
} point;

/*
 * Initialize model
 */
model *get_parameters (void);

/*
 * Function to minimize
 */
void cost (int, point *, const model *);
