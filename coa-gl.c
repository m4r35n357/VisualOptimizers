#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "cut.h"

static box *b1, *b2;
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
            c.clamp = true;
            coa(b1, m, c);
            c.clamp = false;
            coa(b2, m, c);
            get_vertices(v1, b1->p);
            get_vertices(v2, b2->p);
        }
        if (stepping) paused = true;
    }

    draw_grid();

    rgb box1_colour = get_colour(DARK_RED);
    float x_lower = (float)b1->lower[0], x_upper = (float)b1->upper[0];
    float y_lower = (float)b1->lower[1], y_upper = (float)b1->upper[1];
    float z_lower = (float)b1->lower[2], z_upper = (float)b1->upper[2];
    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_upper, y_lower, z_lower}, box1_colour);
    line((gl_point){x_lower, y_lower, z_upper}, (gl_point){x_upper, y_lower, z_upper}, box1_colour);
    line((gl_point){x_lower, y_upper, z_lower}, (gl_point){x_upper, y_upper, z_lower}, box1_colour);
    line((gl_point){x_lower, y_upper, z_upper}, (gl_point){x_upper, y_upper, z_upper}, box1_colour);

    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_lower, y_upper, z_lower}, box1_colour);
    line((gl_point){x_lower, y_lower, z_upper}, (gl_point){x_lower, y_upper, z_upper}, box1_colour);
    line((gl_point){x_upper, y_lower, z_lower}, (gl_point){x_upper, y_upper, z_lower}, box1_colour);
    line((gl_point){x_upper, y_lower, z_upper}, (gl_point){x_upper, y_upper, z_upper}, box1_colour);

    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_lower, y_lower, z_upper}, box1_colour);
    line((gl_point){x_lower, y_upper, z_lower}, (gl_point){x_lower, y_upper, z_upper}, box1_colour);
    line((gl_point){x_upper, y_lower, z_lower}, (gl_point){x_upper, y_lower, z_upper}, box1_colour);
    line((gl_point){x_upper, y_upper, z_lower}, (gl_point){x_upper, y_upper, z_upper}, box1_colour);

    rgb box2_colour = get_colour(DARK_MAGENTA);
    x_lower = (float)b2->lower[0], x_upper = (float)b2->upper[0];
    y_lower = (float)b2->lower[1], y_upper = (float)b2->upper[1];
    z_lower = (float)b2->lower[2], z_upper = (float)b2->upper[2];
    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_upper, y_lower, z_lower}, box2_colour);
    line((gl_point){x_lower, y_lower, z_upper}, (gl_point){x_upper, y_lower, z_upper}, box2_colour);
    line((gl_point){x_lower, y_upper, z_lower}, (gl_point){x_upper, y_upper, z_lower}, box2_colour);
    line((gl_point){x_lower, y_upper, z_upper}, (gl_point){x_upper, y_upper, z_upper}, box2_colour);

    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_lower, y_upper, z_lower}, box2_colour);
    line((gl_point){x_lower, y_lower, z_upper}, (gl_point){x_lower, y_upper, z_upper}, box2_colour);
    line((gl_point){x_upper, y_lower, z_lower}, (gl_point){x_upper, y_upper, z_lower}, box2_colour);
    line((gl_point){x_upper, y_lower, z_upper}, (gl_point){x_upper, y_upper, z_upper}, box2_colour);

    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_lower, y_lower, z_upper}, box2_colour);
    line((gl_point){x_lower, y_upper, z_lower}, (gl_point){x_lower, y_upper, z_upper}, box2_colour);
    line((gl_point){x_upper, y_lower, z_lower}, (gl_point){x_upper, y_lower, z_upper}, box2_colour);
    line((gl_point){x_upper, y_upper, z_lower}, (gl_point){x_upper, y_upper, z_upper}, box2_colour);

    if (targets && minimum) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(WHITE));
        }
    }

    for (int i = 0; i < c.m; i++) {
        ball(v1[i], b1->p[i] == b1->best ? get_colour(LIGHT_RED) : get_colour(DARK_GREEN));
        ball(v2[i], b2->p[i] == b2->best ? get_colour(LIGHT_MAGENTA) : get_colour(DARK_CYAN));
    }

    if (osd_active) {
        sprintf(hud1, c.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                b1->iterations, b1->evaluations,
                c.places, b1->best->x[0], c.places, b1->best->x[1], c.places, b1->best->x[2], c.places, b1->best->f);
        sprintf(hud2, c.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                b2->iterations, b2->evaluations,
                c.places, b2->best->x[0], c.places, b2->best->x[1], c.places, b2->best->x[2], c.places, b2->best->f);
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

void CloseWindow () {
    point *best = b1->p[0]->f <= b2->p[0]->f ? *b1->p : *b2->p;
    // print solution 1
    fprintf(stderr, "%s%s  Clamped%s ", *b1->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, "  %5d %6d  ", b1->iterations, b1->evaluations);
    print_result(c.n, *b1->p, c.places, c.fmt);
    // print solution 2
    fprintf(stderr, "%s%sUnclamped%s ", *b2->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, "  %5d %6d  ", b2->iterations, b2->evaluations);
    print_result(c.n, *b2->p, c.places, c.fmt);
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    c = get_config(argv, true);

    // model parameters
    m = model_init();

    b1 = get_box(m, c);
    b2 = get_box(m, c);

    // get minima for targets if known
    targets = get_known_minima();

    v1 = malloc((size_t)c.m * sizeof (gl_point)); CHECK(v1);
    get_vertices(v1, b1->p);
    v2 = malloc((size_t)c.m * sizeof (gl_point)); CHECK(v2);
    get_vertices(v2, b2->p);

    lower = (float)c.lower;
    upper = (float)c.upper;
    centre = (float)(0.5L * (c.lower + c.upper));
    radius = 1.5F * ((float)c.upper - (float)c.lower);
    ball_size = 0.004F * ((float)c.upper - (float)c.lower);

    ApplicationInit(argc, argv, "Optimization By Cut Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
