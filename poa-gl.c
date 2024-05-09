#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "particles.h"

static population *p1, *p2;
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
            c.mode = 1;  // Random
            coa(p1, m, &c);
            c.mode = 0;  // Unclamped (except to bounding box)
            coa(p2, m, &c);
            get_vertices(v1, p1->agents);
            get_vertices(v2, p2->agents);
        }
        if (stepping) paused = true;
    }

    draw_grid();

    if (targets && minimum) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(WHITE));
        }
    }

    cut_box((float)p1->lower[0], (float)p1->lower[1], (float)p1->lower[2],
            (float)p1->upper[0], (float)p1->upper[1], (float)p1->upper[2], get_colour(DARK_RED));
    cut_box((float)p2->lower[0], (float)p2->lower[1], (float)p2->lower[2],
            (float)p2->upper[0], (float)p2->upper[1], (float)p2->upper[2], get_colour(DARK_MAGENTA));
    for (int i = 0; i < c.m; i++) {
        ball(v1[i], p1->agents[i] == p1->best ? get_colour(LIGHT_RED) : get_colour(DARK_GREEN));
        ball(v2[i], p2->agents[i] == p2->best ? get_colour(LIGHT_MAGENTA) : get_colour(DARK_CYAN));
    }

    if (osd_active) {
        osd_status(hud1, c.fmt, p1->iterations, p1->evaluations, c.places, p1->best);
        osd_status(hud2, c.fmt, p2->iterations, p2->evaluations, c.places, p2->best);
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
    point *best = p1->agents[0]->f <= p2->agents[0]->f ? *p1->agents : *p2->agents;
    // print solution 1
    fprintf(stderr, "%s%sRandom%s ", *p1->agents == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, "  %5d %6d  ", p1->iterations, p1->evaluations);
    print_result(c.n, *p1->agents, c.places, c.fmt);
    // print solution 2
    fprintf(stderr, "%s%s   Cut%s ", *p2->agents == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, "  %5d %6d  ", p2->iterations, p2->evaluations);
    print_result(c.n, *p2->agents, c.places, c.fmt);
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);

    // options
    c = get_config(argv);

    // model parameters
    m = model_init();

    p1 = get_box(m, &c);
    p2 = get_box(m, &c);

    // get minima for targets if known
    targets = get_known_minima();

    v1 = malloc((size_t)c.m * sizeof (gl_point)); CHECK(v1);
    get_vertices(v1, p1->agents);
    v2 = malloc((size_t)c.m * sizeof (gl_point)); CHECK(v2);
    get_vertices(v2, p2->agents);

    lower = (float)c.lower;
    upper = (float)c.upper;
    middle = (float)(0.5L * (c.lower + c.upper));
    radius = 1.5F * ((float)c.upper - (float)c.lower);
    ball_size = 0.004F * ((float)c.upper - (float)c.lower);

    ApplicationInit(argc, argv, "Random & Optimization By Cut Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
