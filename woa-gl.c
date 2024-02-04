#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "whale.h"

static population *p;
static model *m;
static minima *targets;
static options o;
static gl_point *v;
static real min, max;

static gl_point get_gl_point (real *point) {
    return (gl_point){(float)point[0], (float)point[1], (float)point[2]};
}

static void get_vertices (gl_point *vertices, point **whales) {
    for (int i = 0; i < o.whales; i++) {
        vertices[i] = get_gl_point(whales[i]->x);
    }
}

void Animate () {
    SetupView();

    if (!paused) {
        if (initial) {
            initial = false;
        } else {
            if (!finished1) {
                woa(p, min, max, m, o);
                get_vertices(v, p->whales);
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

    for (int i = 0; i < o.whales; i++) {
        ball(v[i], p->whales[i] == p->prey ? get_colour(DARK_RED) : get_colour(DARK_GREEN));
    }

    if (osd_active) {
        sprintf(hud, o.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                p->iterations, p->evaluations,
                o.places, p->prey->x[0], o.places, p->prey->x[1], o.places, p->prey->x[2], o.places, p->prey->f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_YELLOW), hud);
    }

    ReDraw();
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // options
    o = get_options(argv, true);
    min = strtold(argv[6], NULL);
    max = strtold(argv[7], NULL);

    // model parameters
    m = model_init();

    p = get_population(min, max, m, o);

    // get minima for targets if known
    targets = get_known_minima();

    v = malloc((size_t)o.whales * sizeof (gl_point)); CHECK(v);
    get_vertices(v, p->whales);

    radius = 1.5F * ((float)max - (float)min);
    ball_size = 0.005F * ((float)max - (float)min);;

    ApplicationInit(argc, argv, "Whale Optimization Visualizer");
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.
    return 0 ;          // Compiler requires this to be here. (Never reached)
}
