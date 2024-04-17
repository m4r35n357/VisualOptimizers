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

static void cut_box (float x_l, float y_l, float z_l, float x_u, float y_u, float z_u, rgb colour) {
    line((gl_point){x_l, y_l, z_l}, (gl_point){x_u, y_l, z_l}, colour);
    line((gl_point){x_l, y_l, z_u}, (gl_point){x_u, y_l, z_u}, colour);
    line((gl_point){x_l, y_u, z_l}, (gl_point){x_u, y_u, z_l}, colour);
    line((gl_point){x_l, y_u, z_u}, (gl_point){x_u, y_u, z_u}, colour);

    line((gl_point){x_l, y_l, z_l}, (gl_point){x_l, y_u, z_l}, colour);
    line((gl_point){x_l, y_l, z_u}, (gl_point){x_l, y_u, z_u}, colour);
    line((gl_point){x_u, y_l, z_l}, (gl_point){x_u, y_u, z_l}, colour);
    line((gl_point){x_u, y_l, z_u}, (gl_point){x_u, y_u, z_u}, colour);

    line((gl_point){x_l, y_l, z_l}, (gl_point){x_l, y_l, z_u}, colour);
    line((gl_point){x_l, y_u, z_l}, (gl_point){x_l, y_u, z_u}, colour);
    line((gl_point){x_u, y_l, z_l}, (gl_point){x_u, y_l, z_u}, colour);
    line((gl_point){x_u, y_u, z_l}, (gl_point){x_u, y_u, z_u}, colour);
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

    if (targets && minimum) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(WHITE));
        }
    }

    cut_box((float)b1->lower[0], (float)b1->lower[1], (float)b1->lower[2],
            (float)b1->upper[0], (float)b1->upper[1], (float)b1->upper[2], get_colour(DARK_RED));
    cut_box((float)b2->lower[0], (float)b2->lower[1], (float)b2->lower[2],
            (float)b2->upper[0], (float)b2->upper[1], (float)b2->upper[2], get_colour(DARK_MAGENTA));

    for (int i = 0; i < c.m; i++) {
        ball(v1[i], b1->p[i] == b1->best ? get_colour(LIGHT_RED) : get_colour(DARK_GREEN));
        ball(v2[i], b2->p[i] == b2->best ? get_colour(LIGHT_MAGENTA) : get_colour(DARK_CYAN));
    }

    if (osd_active) {
        osd_status (hud1, c.fmt, b1->iterations, b1->evaluations, c.places, b1->best);
        osd_status (hud2, c.fmt, b2->iterations, b2->evaluations, c.places, b2->best);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_GREEN), hud1);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 40, get_colour(DARK_CYAN), hud2);
        if (targets && minimum) {
            osd_status (hud3, c.fmt, 0, 0, c.places, targets->min);
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
