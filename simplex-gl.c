#include <stdio.h>
#include <math.h>
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
            nelder_mead(s1, m, &o);
            nelder_mead(s2, m, &o);
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

    if (centroid) {
        for (int i = 0; i < 3; i++) {
            line(get_gl_point(s1->centroid->x), v1[i], get_colour(DARK_MAGENTA));
            line(get_gl_point(s2->centroid->x), v2[i], get_colour(DARK_MAGENTA));
        }
    }

    if (targets && minimum) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(WHITE));
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
        sprintf(hud1, o.fmt ? "%3d %3d [ % .*Le % .*Le % .*Le ] % .*Le" : "%3d %3d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s1->iterations, s1->evaluations,
                o.places, s1->p[0].x[0], o.places, s1->p[0].x[1], o.places, s1->p[0].x[2], o.places, s1->p[0].f);
        sprintf(hud2, o.fmt ? "%3d %3d [ % .*Le % .*Le % .*Le ] % .*Le" : "%3d %3d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s2->iterations, s2->evaluations,
                o.places, s2->p[0].x[0], o.places, s2->p[0].x[1], o.places, s2->p[0].x[2], o.places, s2->p[0].f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_CYAN), hud1);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 40, get_colour(DARK_YELLOW), hud2);
        if (targets && minimum) {
            sprintf(hud3, o.fmt ? "%3s %3s [ % .*Le % .*Le % .*Le ] % .*Le" : "%3s %3s [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                    " ", " ",
                    o.places, targets->min[0].x[0], o.places, targets->min[0].x[1], o.places, targets->min[0].x[2],
                    o.places, targets->min[0].f);
            osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 60, get_colour(LIGHT_GREY), hud3);
        }
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
    CHECK(argc >= 9);

    // optimizer settings
    o = get_settings(argv, true);

    // model parameters
    m = model_init();

    point *start = get_point(o.n);
    if (o.init_mode) {  // random initial point in range
        CHECK(argc == 11);
        o.lower = strtold(argv[9], NULL);
        o.upper = strtold(argv[10], NULL);  CHECK(o.upper > o.lower);
        set_random_coordinates(start, o.n, o.lower, o.upper);
    } else {  // set initial point from command arguments
        CHECK(argc == 9 + o.n);
        real max = 0.0L;
        for (int j = 0; j < o.n; j++) {
            start->x[j] = strtold(argv[9 + j], NULL);
            if (fabsl(start->x[j]) > max) max = fabsl(start->x[j]);
        }
        o.upper = 2.0L * max;
        o.lower = -o.upper;
    }
    cost(o.n, start, m);

    // default simplex . . .
    s1 = nm_simplex(o.n, o.size, start, o.adaptive);
    for (int i = 0; i < s1->n + 1; i++) {  // initial cost at simplex vertices
        cost(s1->n, s1->p + i, m);
        s1->evaluations++;
    }
    sort(s1);

    // . . . and its "dual"
    s2 = nm_simplex(o.n, o.size, start, o.adaptive);
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

    lower = (float)o.lower;
    upper = (float)o.upper;
    centre = (float)(0.5L * (o.lower + o.upper));
    radius = 1.5F * ((float)o.upper - (float)o.lower);
    ball_size = 0.002F * ((float)o.upper - (float)o.lower);

    ApplicationInit(argc, argv, "Nelder-Mead Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
