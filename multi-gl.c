#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "simplex.h"

static simplex *s1, *s2;
static model *m;
static minima *targets;
static optimset o;
static gl_point *v1, *v2;

static gl_point get_gl_point (real *p) {
    return (gl_point){(float)p[0], (float)p[1], (float)p[2]};
}

static void get_vertices (gl_point *vertices, point *points) {
    for (int i = 0; i < 4; i++) {
        vertices[i] = get_gl_point(points[i].x);
    }
}

void Animate () {
    SetupView();

    if (!paused) {
        if (initial) {
            initial = false;
        } else {
            if (!finished1) {
                multidirectional_search(s1, m, &o);
                get_vertices(v1, s1->p);
            }
            if (!finished2) {
                multidirectional_search(s2, m, &o);
                get_vertices(v2, s2->p);
            }
        }
        if (stepping) paused = true;
    }

    float fmin = (float)o.lower, fmax = (float)o.upper;
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

    for (int i = 0; i < 4; i++) {
        for (int k = i; k < 4; k++) {
            line(v1[i], v1[k], get_colour(DARK_GREY));
            line(v2[i], v2[k], get_colour(DARK_GREY));
        }
        ball(v1[i], !i ? get_colour(LIGHT_GREEN) : (i == 3 ? get_colour(LIGHT_RED) : get_colour(DARK_CYAN)));
        ball(v2[i], !i ? get_colour(LIGHT_GREEN) : (i == 3 ? get_colour(LIGHT_RED) : get_colour(DARK_YELLOW)));
    }

    if (osd_active) {
        sprintf(hud1, o.fmt ? "%.3d %.3d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.3d %.3d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s1->iterations, s1->evaluations,
                o.places, s1->p[0].x[0], o.places, s1->p[0].x[1], o.places, s1->p[0].x[2], o.places, s1->p[0].f);
        sprintf(hud2, o.fmt ? "%.3d %.3d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.3d %.3d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s2->iterations, s2->evaluations,
                o.places, s2->p[0].x[0], o.places, s2->p[0].x[1], o.places, s2->p[0].x[2], o.places, s2->p[0].f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_CYAN), hud1);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 40, get_colour(DARK_YELLOW), hud2);
    }

    ReDraw();
}

void CloseWindow () {
    point *best = s1->p[0].f <= s2->p[0].f ? s1->p : s2->p;
    // print solution 1
    fprintf(stderr, "%s%s1%s ", s1->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s1->iterations, s1->evaluations);
    print_result(s1->n, s1->p, o.places, o.fmt);
    // print solution 2
    fprintf(stderr, "%s%s2%s ", s2->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s2->iterations, s2->evaluations);
    print_result(s2->n, s2->p, o.places, o.fmt);
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 8);

    // optimizer settings
    o = get_settings(argv, true);

    // model parameters
    m = model_init();

    point *start = get_point(o.n);
    if (o.init_mode) {  // random initial point in range
        CHECK(argc == 10);
        o.lower = strtold(argv[8], NULL);
        o.upper = strtold(argv[9], NULL);  CHECK(o.upper > o.lower);
        set_random_coordinates(start, o.n, o.lower, o.upper);
    } else {  // set initial point from command arguments
        CHECK(argc == 8 + o.n);
        for (int j = 0; j < o.n; j++) {
            start->x[j] = strtold(argv[8 + j], NULL);
        }
    }
    cost(o.n, start, m);

    // default simplex . . .
    s1 = mds_simplex(o.n, o.size, start);
    for (int i = 0; i < s1->n + 1; i++) {  // initial cost at simplex vertices
        cost(s1->n, s1->p + i, m);
        s1->evaluations++;
    }
    sort(s1);

    // . . . and its "dual"
    s2 = mds_simplex(o.n, o.size, start);
    for (int i = 0; i < s2->n + 1; i++) {  // form "dual" by projecting vertices through the centre
        project(s2->p + i, s2, m, 1.0L, s2->p + i, start);
    }
    sort(s2);

    // print starting point
    fprintf(stderr, "%s       Initial  ", GRY);
    print_result(o.n, start, o.places, o.fmt);
    fprintf(stderr, o.fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter%s    % .*Lf\n",
            GRY, NRM, o.places, distance(s1->n, s1->p, s1->p + s1->n));

    // get minima for targets if known
    targets = get_known_minima();

    v1 = malloc(4 * sizeof (gl_point)); CHECK(v1);
    get_vertices(v1, s1->p);
    v2 = malloc(4 * sizeof (gl_point)); CHECK(v2);
    get_vertices(v2, s2->p);

    radius = 1.5F * ((float)o.upper - (float)o.lower);
    ball_size = 0.002F * ((float)o.upper - (float)o.lower);

    ApplicationInit(argc, argv, "Nelder-Mead Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
