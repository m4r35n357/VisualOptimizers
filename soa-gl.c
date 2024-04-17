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
            c.convergence = false;
            soa(s1, m, c);
            c.convergence = true;
            soa(s2, m, c);
            get_vertices(v1, s1->p);
            get_vertices(v2, s2->p);
        }
        if (stepping) paused = true;
    }

    draw_grid();

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
        osd_status(hud1, c.fmt, s1->k, s1->evaluations, c.places, s1->x_star);
        osd_status(hud2, c.fmt, s2->k, s2->evaluations, c.places, s2->x_star);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_GREEN), hud1);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 40, get_colour(DARK_CYAN), hud2);
        if (targets && minimum) {
            osd_status(hud3, c.fmt, 0, 0, c.places, targets->min);
            osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 60, get_colour(LIGHT_GREY), hud3);
        }
    }

    ReDraw();
}

void CloseWindow () {
    point *best = s1->p[0]->f <= s2->p[0]->f ? *s1->p : *s2->p;
    // print solution 1
    fprintf(stderr, "%s%s    Descent%s ", *s1->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, "  %5d %6d  ", s1->k, s1->evaluations);
    print_result(c.n, *s1->p, c.places, c.fmt);
    // print solution 2
    fprintf(stderr, "%s%sConvergence%s ", *s2->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, "  %5d %6d  ", s2->k, s2->evaluations);
    print_result(c.n, *s2->p, c.places, c.fmt);
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

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
    ball_size = 0.004F * ((float)c.upper - (float)c.lower);

    ApplicationInit(argc, argv, "Spiral Optimization Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
