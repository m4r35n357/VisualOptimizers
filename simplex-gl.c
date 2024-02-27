#include <stdio.h>
#include <time.h>
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
                nelder_mead(s1, m, &o);
                get_vertices(v1, s1->p);
            }
            if (!finished2) {
                nelder_mead(s2, m, &o);
                get_vertices(v2, s2->p);
            }
        }
        if (stepping) paused = true;
    }

    float fmin = (float)o.lower, fmax = (float)o.upper;
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

    if (centroid) {
        for (int i = 0; i < 3; i++) {
            line(get_gl_point(s1->centroid->x), v1[i], get_colour(DARK_MAGENTA));
            line(get_gl_point(s2->centroid->x), v2[i], get_colour(DARK_MAGENTA));
        }
    }

    if (targets) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(LIGHT_YELLOW));
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int k = i; k < 4; k++) {
            line(v1[i], v1[k], get_colour(DARK_CYAN));
            line(v2[i], v2[k], get_colour(DARK_CYAN));
        }
        ball(v1[i], !i ? get_colour(LIGHT_GREEN) : (i == 3 ? get_colour(LIGHT_RED) : get_colour(LIGHT_GREY)));
        ball(v2[i], !i ? get_colour(LIGHT_GREEN) : (i == 3 ? get_colour(LIGHT_RED) : get_colour(LIGHT_GREY)));
    }

    if (osd_active) {
        point *best = s1->p[0].f <= s2->p[0].f ? s1->p : s2->p;
        sprintf(hud, o.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s1->iterations, s1->evaluations,
                o.places, best->x[0], o.places, best->x[1], o.places, best->x[2], o.places, best->f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_YELLOW), hud);
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

    point *start;
    if (o.random_init) {  // random initial point in range
        srand((unsigned int)time(NULL));
        o.lower = strtold(argv[9], NULL),
        o.upper = strtold(argv[10], NULL),
        start = get_random_point(o.n, o.lower, o.upper);
        cost(o.n, start, m);
        point *trial = get_point(o.n);
        for (int i = 0; i < o.random_init * o.n; i++) {
            get_random_coordinates(trial, o.n, o.lower, o.upper);
            cost(o.n, trial, m);
            if (trial->f < start->f) copy_point(o.n, trial, start);
        }
    } else {  // set initial point from command arguments
        start = get_point(o.n);
        for (int j = 0; j < o.n; j++) {
            start->x[j] = strtold(argv[9 + j], NULL);
        }
    }

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

    radius = 1.5F * ((float)o.upper - (float)o.lower);
    ball_size = 0.002F * ((float)o.upper - (float)o.lower);

    ApplicationInit(argc, argv, "Nelder-Mead Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
