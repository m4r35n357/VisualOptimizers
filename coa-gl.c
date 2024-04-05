#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "cut.h"

static box *b;
static model *m;
static minima *targets;
static config c;
static gl_point *v1;

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
            coa(b, m, c);
            get_vertices(v1, b->p);
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

    rgb cut_colour = get_colour(DARK_YELLOW);
    float x_lower = (float)b->x_l[0], x_upper = (float)b->x_u[0];
    float y_lower = (float)b->x_l[1], y_upper = (float)b->x_u[1];
    float z_lower = (float)b->x_l[2], z_upper = (float)b->x_u[2];
    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_upper, y_lower, z_lower}, cut_colour);
    line((gl_point){x_lower, y_lower, z_upper}, (gl_point){x_upper, y_lower, z_upper}, cut_colour);
    line((gl_point){x_lower, y_upper, z_lower}, (gl_point){x_upper, y_upper, z_lower}, cut_colour);
    line((gl_point){x_lower, y_upper, z_upper}, (gl_point){x_upper, y_upper, z_upper}, cut_colour);

    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_lower, y_upper, z_lower}, cut_colour);
    line((gl_point){x_lower, y_lower, z_upper}, (gl_point){x_lower, y_upper, z_upper}, cut_colour);
    line((gl_point){x_upper, y_lower, z_lower}, (gl_point){x_upper, y_upper, z_lower}, cut_colour);
    line((gl_point){x_upper, y_lower, z_upper}, (gl_point){x_upper, y_upper, z_upper}, cut_colour);

    line((gl_point){x_lower, y_lower, z_lower}, (gl_point){x_lower, y_lower, z_upper}, cut_colour);
    line((gl_point){x_lower, y_upper, z_lower}, (gl_point){x_lower, y_upper, z_upper}, cut_colour);
    line((gl_point){x_upper, y_lower, z_lower}, (gl_point){x_upper, y_lower, z_upper}, cut_colour);
    line((gl_point){x_upper, y_upper, z_lower}, (gl_point){x_upper, y_upper, z_upper}, cut_colour);

    if (targets && minimum) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(WHITE));
        }
    }

    for (int i = 0; i < c.m; i++) {
        ball(v1[i], b->p[i] == b->best ? get_colour(LIGHT_RED) : get_colour(DARK_GREEN));
    }

    if (osd_active) {
        sprintf(hud1, c.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                b->k, b->evaluations,
                c.places, b->best->x[0], c.places, b->best->x[1], c.places, b->best->x[2], c.places, b->best->f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_GREEN), hud1);
        if (targets && minimum) {
            sprintf(hud3, c.fmt ? "%3s %5s [ % .*Le % .*Le % .*Le ] % .*Le" : "%3s %5s [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                    " ", " ",
                    c.places, targets->min[0].x[0], c.places, targets->min[0].x[1], c.places, targets->min[0].x[2],
                    c.places, targets->min[0].f);
            osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 40, get_colour(LIGHT_GREY), hud3);
        }
    }

    ReDraw();
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // options
    c = get_config(argv, true);

    // model parameters
    m = model_init();

    b = get_box(m, c);

    // get minima for targets if known
    targets = get_known_minima();

    v1 = malloc((size_t)c.m * sizeof (gl_point)); CHECK(v1);
    get_vertices(v1, b->p);

    lower = (float)c.lower;
    upper = (float)c.upper;
    centre = (float)(0.5L * (c.lower + c.upper));
    radius = 1.5F * ((float)c.upper - (float)c.lower);
    ball_size = 0.005F * ((float)c.upper - (float)c.lower);

    ApplicationInit(argc, argv, "Optimization By Cut Visualizer");
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
