
typedef struct Whale {
    double *x;
    double value;
} whale;

whale *create_whale (int dim, double min_x, double max_x);

double *woa (int max_i, int n, int dim, double min_x, double max_x);

double cost (double *x, int dim); // Placeholder for the actual cost function

int randint (int n);
