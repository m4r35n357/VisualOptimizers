#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "spiral.h"

static spiral *s;
static model *m;
static minima *targets;
static config c;
static gl_point *v;
static real min, max;

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
            if (!finished1) {
                soa(s, m, c);
                get_vertices(v, s->points);
            }
        }
        if (stepping) paused = true;
    }

    float fmin = (float)min, fmax = (float)max;
    rgb box_colour = get_colour(DARK_BLUE);
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

    rgb axis_colour = get_colour(DARK_GREY);
    line((gl_point){fmin, 0.0F, 0.0F}, (gl_point){fmax, 0.0F, 0.0F}, axis_colour);
    line((gl_point){0.0F, fmin, 0.0F}, (gl_point){0.0F, fmax, 0.0F}, axis_colour);
    line((gl_point){0.0F, 0.0F, fmin}, (gl_point){0.0F, 0.0F, fmax}, axis_colour);

    if (targets) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(LIGHT_YELLOW));
        }
    }

    for (int i = 0; i < c.m; i++) {
        ball(v[i], get_colour(DARK_GREEN));
    }

    if (osd_active) {
        sprintf(hud, c.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s->k, s->evaluations,
                c.places, s->x_star->x[0], c.places, s->x_star->x[1], c.places, s->x_star->x[2], c.places, s->x_star->f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_YELLOW), hud);
    }

    ReDraw();
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    c = get_config(argv, true);
    min = strtold(argv[7], NULL);
    max = strtold(argv[8], NULL);

    // model parameters
    m = model_init();

    s = get_spiral(min, max, m, c);

    // get minima for targets if known
    targets = get_known_minima();
    printf("Hi!\n");

    v = malloc((size_t)c.m * sizeof (gl_point)); CHECK(v);
    get_vertices(v, s->points);
    printf("Ho!\n");

    radius = 1.5F * ((float)max - (float)min);
    ball_size = 0.005F * ((float)max - (float)min);;

    ApplicationInit(argc, argv, "Spiral Optimization Visualizer");
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.
    return 0 ;          // Compiler requires this to be here. (Never reached)
}
