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

    rgb box_colour = get_colour(LIGHT_BLUE);
    line((gl_point){lower, lower, lower}, (gl_point){upper, lower, lower}, box_colour);
    line((gl_point){lower, lower, upper}, (gl_point){upper, lower, upper}, box_colour);
    line((gl_point){lower, upper, lower}, (gl_point){upper, upper, lower}, box_colour);
    line((gl_point){lower, upper, upper}, (gl_point){upper, upper, upper}, box_colour);

    line((gl_point){lower, lower, lower}, (gl_point){lower, upper, lower}, box_colour);
    line((gl_point){lower, lower, upper}, (gl_point){lower, upper, upper}, box_colour);
    line((gl_point){upper, lower, lower}, (gl_point){upper, upper, lower}, box_colour);
    line((gl_point){upper, lower, upper}, (gl_point){upper, upper, upper}, box_colour);

    line((gl_point){lower, lower, lower}, (gl_point){lower, lower, upper}, box_colour);
    line((gl_point){lower, upper, lower}, (gl_point){lower, upper, upper}, box_colour);
    line((gl_point){upper, lower, lower}, (gl_point){upper, lower, upper}, box_colour);
    line((gl_point){upper, upper, lower}, (gl_point){upper, upper, upper}, box_colour);

    rgb axis_colour = get_colour(DARK_BLUE);
    line((gl_point){lower, centre, centre}, (gl_point){upper, centre, centre}, axis_colour);
    line((gl_point){centre, lower, centre}, (gl_point){centre, upper, centre}, axis_colour);
    line((gl_point){centre, centre, lower}, (gl_point){centre, centre, upper}, axis_colour);

    line((gl_point){upper, lower, centre}, (gl_point){upper, upper, centre}, axis_colour);
    line((gl_point){upper, centre, lower}, (gl_point){upper, centre, upper}, axis_colour);
    line((gl_point){lower, lower, centre}, (gl_point){lower, upper, centre}, axis_colour);
    line((gl_point){lower, centre, lower}, (gl_point){lower, centre, upper}, axis_colour);

    line((gl_point){lower, upper, centre}, (gl_point){upper, upper, centre}, axis_colour);
    line((gl_point){centre, upper, lower}, (gl_point){centre, upper, upper}, axis_colour);
    line((gl_point){lower, lower, centre}, (gl_point){upper, lower, centre}, axis_colour);
    line((gl_point){centre, lower, lower}, (gl_point){centre, lower, upper}, axis_colour);

    line((gl_point){lower, centre, upper}, (gl_point){upper, centre, upper}, axis_colour);
    line((gl_point){centre, lower, upper}, (gl_point){centre, upper, upper}, axis_colour);
    line((gl_point){lower, centre, lower}, (gl_point){upper, centre, lower}, axis_colour);
    line((gl_point){centre, lower, lower}, (gl_point){centre, upper, lower}, axis_colour);

    if (targets && minimum) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(WHITE));
        }
    }

    for (int i = 0; i < c.m; i++) {
        ball(v1[i], s1->p[i] == s1->x_star ? get_colour(LIGHT_RED) : get_colour(DARK_GREEN));
        ball(v2[i], s2->p[i] == s2->x_star ? get_colour(LIGHT_MAGENTA) : get_colour(DARK_CYAN));
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
        if (targets && minimum) {
            sprintf(hud3, c.fmt ? "%3s %5s [ % .*Le % .*Le % .*Le ] % .*Le" : "%3s %5s [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                    " ", " ",
                    c.places, targets->min[0].x[0], c.places, targets->min[0].x[1], c.places, targets->min[0].x[2],
                    c.places, targets->min[0].f);
            osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 60, get_colour(LIGHT_GREY), hud3);
        }
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

    lower = (float)c.lower;
    upper = (float)c.upper;
    centre = (float)(0.5L * (c.lower + c.upper));
    radius = 1.5F * ((float)c.upper - (float)c.lower);
    ball_size = 0.005F * ((float)c.upper - (float)c.lower);

    ApplicationInit(argc, argv, "Spiral Optimization Visualizer");
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
