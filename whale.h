
typedef struct Whale {
    double *x;
    double value;
} whale;

whale *create_whale (int dim, double min_x, double max_x, int seed, double (*cost)(double *, int));

void free_whale (whale *w);

double *woa (double (*cost)(double *, int), int max_i, int n, int dim, double min_x, double max_x);

//double cost_function(double *x, int dim); // Placeholder for the actual cost function

int randint (int n);
