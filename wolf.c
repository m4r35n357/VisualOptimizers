#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double fitness_rastrigin(double *position, int dim) {
    double fitness_value = 0.0;
    for (int i = 0; i < dim; ++i) {
        double xi = position[i];
        fitness_value += (xi * xi) - (10 * cos(2 * M_PI * xi)) + 10;
    }
    return fitness_value;
}

double fitness_sphere(double *position, int dim) {
    double fitness_value = 0.0;
    for (int i = 0; i < dim; ++i) {
        double xi = position[i];
        fitness_value += (xi * xi);
    }
    return fitness_value;
}

typedef struct {
    double *position;
    double fitness;
} Wolf;

Wolf *create_wolf(int dim, double minx, double maxx, unsigned int seed) {
    Wolf *wolf = (Wolf *)malloc(sizeof(Wolf));
    wolf->position = (double *)malloc(dim * sizeof(double));
    srand(seed);
    for (int i = 0; i < dim; ++i) {
        wolf->position[i] = ((maxx - minx) * ((double)rand() / RAND_MAX) + minx);
    }
    return wolf;
}

void free_wolf(Wolf *wolf) {
    free(wolf->position);
    free(wolf);
}

int compare_wolves(const void *a, const void *b) {
    Wolf *wolfA = *(Wolf **)a;
    Wolf *wolfB = *(Wolf **)b;
    return (wolfA->fitness > wolfB->fitness) - (wolfA->fitness < wolfB->fitness);
}

double *gwo(double (*fitness)(double *, int), int max_iter, int n, int dim, double minx, double maxx) {
    Wolf **population = (Wolf **)malloc(n * sizeof(Wolf *));
    for (int i = 0; i < n; ++i) {
        population[i] = create_wolf(dim, minx, maxx, i);
        population[i]->fitness = fitness(population[i]->position, dim);
    }

    qsort(population, n, sizeof(Wolf *), compare_wolves);

    Wolf *alpha_wolf = population[0];
    Wolf *beta_wolf = population[1];
    Wolf *gamma_wolf = population[2];

    int Iter = 0;
    while (Iter < max_iter) {
        if (Iter % 10 == 0 && Iter > 1) {
            printf("Iter = %d best fitness = %.3f\n", Iter, alpha_wolf->fitness);
        }

        double a = 2 * (1 - ((double)Iter / max_iter));

        for (int i = 0; i < n; ++i) {
            double A1 = a * (2 * ((double)rand() / RAND_MAX) - 1);
            double A2 = a * (2 * ((double)rand() / RAND_MAX) - 1);
            double A3 = a * (2 * ((double)rand() / RAND_MAX) - 1);
            double C1 = 2 * ((double)rand() / RAND_MAX);
            double C2 = 2 * ((double)rand() / RAND_MAX);
            double C3 = 2 * ((double)rand() / RAND_MAX);
            double *Xnew = (double *)malloc(dim * sizeof(double));
            for (int j = 0; j < dim; ++j) {
                double X1 = alpha_wolf->position[j] - A1 * fabs(C1 * alpha_wolf->position[j] - population[i]->position[j]);
                double X2 = beta_wolf->position[j] - A2 * fabs(C2 * beta_wolf->position[j] - population[i]->position[j]);
                double X3 = gamma_wolf->position[j] - A3 * fabs(C3 * gamma_wolf->position[j] - population[i]->position[j]);
                Xnew[j] = (X1 + X2 + X3) / 3.0;
            }

            double fnew = fitness(Xnew, dim);

            if (fnew < population[i]->fitness) {
                free(population[i]->position);
                population[i]->position = Xnew;
                population[i]->fitness = fnew;
            } else {
                free(Xnew);
            }
        }

        qsort(population, n, sizeof(Wolf *), compare_wolves);

        alpha_wolf = population[0];
        beta_wolf = population[1];
        gamma_wolf = population[2];
        Iter++;
    }

    double *best_position = (double *)malloc(dim * sizeof(double));
    for (int i = 0; i < dim; ++i) {
        best_position[i] = alpha_wolf->position[i];
    }

    for (int i = 0; i < n; ++i) {
        free_wolf(population[i]);
    }
    free(population);

    return best_position;
}

int main() {
    printf("\nBegin grey wolf optimization on rastrigin function\n");
    int dim = 3;
    int num_particles = 50;
    int max_iter = 100;
    printf("Setting num_particles = %d\n", num_particles);
    printf("Setting max_iter = %d\n", max_iter);
    printf("\nStarting GWO algorithm\n");
    double *best_position = gwo(fitness_rastrigin, max_iter, num_particles, dim, -10.0, 10.0);
    printf("\nGWO completed\n");
    printf("\nBest solution found:\n");
    for (int k = 0; k < dim; ++k) {
        printf("%.6f ", best_position[k]);
    }
    double err = fitness_rastrigin(best_position, dim);
    printf("\nfitness of best solution = %.6f\n", err);
    printf("\nEnd GWO for rastrigin\n\n");

    printf("\nBegin grey wolf optimization on sphere function\n");
    best_position = gwo(fitness_sphere, max_iter, num_particles, dim, -10.0, 10.0);
    printf("\nGWO completed\n");
    printf("\nBest solution found:\n");
    for (int k = 0; k < dim; ++k) {
        printf("%.6f ", best_position[k]);
    }
    err = fitness_sphere(best_position, dim);
    printf("\nfitness of best solution = %.6f\n", err);
    printf("\nEnd GWO for sphere\n");

    free(best_position);
    return 0;
}
