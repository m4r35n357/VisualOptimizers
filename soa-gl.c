#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "spiral.h"

static spiral *s1, *s2;
static model *m;
static minima *targets;
static config c;
static gl_point *v1, *v2;

static gl_point get_gl_point (real *p) {
    return (gl_point){(float)p[0], (float)p[1], (float)p[2]};
}

static void get_vertices (gl_point *vertices, point **points) {
    for (int i = 0; i < c.m; i++) {
        vertices[i] = get_gl_point(points[i]->x);
    }
}

void Animate () {
    SetupView();

    if (!paused) {
        if (initial) {
            initial = false;
        } else {
            soa(s1, m, c);
            soa(s2, m, c);
            get_vertices(v1, s1->p);
            get_vertices(v2, s2->p);
        }
        if (stepping) paused = true;
    }

    float fmin = (float)c.lower, fmax = (float)c.upper;
    rgb box_colour = get_colour(LIGHT_BLUE);
    line((gl_point){fmin, fmin, fmin}, (gl_point){fmax, fmin, fmin}, box_colour);
    line((gl_point){fmin, fmin, fmax}, (gl_point){fmax, fmin, fmax}, box_colour);
    line((gl_point){fmin, fmax, fmin}, (gl_point){fmax, fmax, fmin}, box_colour);
    line((gl_point){fmin, fmax, fmax}, (gl_point){fmax, fmax, fmax}, box_colour);

    line((gl_point){fmin, fmin, fmin}, (gl_point){fmin, fmax, fmin}, box_colour);
    line((gl_point){fmin, fmin, fmax}, (gl_point){fmin, fmax, fmax}, box_colour);
    line((gl_point){fmax, fmin, fmin}, (gl_point){fmax, fmax, fmin}, box_colour);
    line((gl_point){fmax, fmin, fmax}, (gl_point){fmax, fmax, fmax}, box_colour);

    line((gl_point){fmin, fmin, fmin}, (gl_point){fmin, fmin, fmax}, box_colour);
    line((gl_point){fmin, fmax, fmin}, (gl_point){fmin, fmax, fmax}, box_colour);
    line((gl_point){fmax, fmin, fmin}, (gl_point){fmax, fmin, fmax}, box_colour);
    line((gl_point){fmax, fmax, fmin}, (gl_point){fmax, fmax, fmax}, box_colour);

    rgb axis_colour = get_colour(DARK_BLUE);
    line((gl_point){fmin, 0.0F, 0.0F}, (gl_point){fmax, 0.0F, 0.0F}, axis_colour);
    line((gl_point){0.0F, fmin, 0.0F}, (gl_point){0.0F, fmax, 0.0F}, axis_colour);
    line((gl_point){0.0F, 0.0F, fmin}, (gl_point){0.0F, 0.0F, fmax}, axis_colour);

    line((gl_point){fmax, fmin, 0.0F}, (gl_point){fmax, fmax, 0.0F}, axis_colour);
    line((gl_point){fmax, 0.0F, fmin}, (gl_point){fmax, 0.0F, fmax}, axis_colour);
    line((gl_point){fmin, fmin, 0.0F}, (gl_point){fmin, fmax, 0.0F}, axis_colour);
    line((gl_point){fmin, 0.0F, fmin}, (gl_point){fmin, 0.0F, fmax}, axis_colour);

    line((gl_point){fmin, fmax, 0.0F}, (gl_point){fmax, fmax, 0.0F}, axis_colour);
    line((gl_point){0.0F, fmax, fmin}, (gl_point){0.0F, fmax, fmax}, axis_colour);
    line((gl_point){fmin, fmin, 0.0F}, (gl_point){fmax, fmin, 0.0F}, axis_colour);
    line((gl_point){0.0F, fmin, fmin}, (gl_point){0.0F, fmin, fmax}, axis_colour);

    line((gl_point){fmin, 0.0F, fmax}, (gl_point){fmax, 0.0F, fmax}, axis_colour);
    line((gl_point){0.0F, fmin, fmax}, (gl_point){0.0F, fmax, fmax}, axis_colour);
    line((gl_point){fmin, 0.0F, fmin}, (gl_point){fmax, 0.0F, fmin}, axis_colour);
    line((gl_point){0.0F, fmin, fmin}, (gl_point){0.0F, fmax, fmin}, axis_colour);

    if (targets && minimum) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(LIGHT_YELLOW));
        }
    }

    for (int i = 0; i < c.m; i++) {
        ball(v1[i], s1->p[i] == s1->x_star ? get_colour(DARK_RED) : get_colour(DARK_GREEN));
        ball(v2[i], s2->p[i] == s2->x_star ? get_colour(DARK_MAGENTA) : get_colour(DARK_CYAN));
    }

    if (osd_active) {
        sprintf(hud1, c.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s1->k, s1->evaluations,
                c.places, s1->x_star->x[0], c.places, s1->x_star->x[1], c.places, s1->x_star->x[2], c.places, s1->x_star->f);
        sprintf(hud2, c.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s2->k, s2->evaluations,
                c.places, s2->x_star->x[0], c.places, s2->x_star->x[1], c.places, s2->x_star->x[2], c.places, s2->x_star->f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_GREEN), hud1);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 40, get_colour(DARK_CYAN), hud2);
    }

    ReDraw();
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 10);

    // options
    c = get_config(argv, true);

    // model parameters
    m = model_init();

    s1 = get_spiral(m, c);
    s2 = get_spiral(m, c);
    for (int i = 0; i < c.m; i++) {
        for (int k = 0; k < c.n; k++) {
            s2->p[i]->x[k] = s1->p[i]->x[k];
        }
    }
    s2->best = s1->best;
    s2->x_star = s1->x_star;
    s2->dual_mode = true;

    // get minima for targets if known
    targets = get_known_minima();

    v1 = malloc((size_t)c.m * sizeof (gl_point)); CHECK(v1);
    get_vertices(v1, s1->p);
    v2 = malloc((size_t)c.m * sizeof (gl_point)); CHECK(v2);
    get_vertices(v2, s2->p);

    radius = 1.5F * ((float)c.upper - (float)c.lower);
    ball_size = 0.005F * ((float)c.upper - (float)c.lower);

    ApplicationInit(argc, argv, "Spiral Optimization Visualizer");
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
