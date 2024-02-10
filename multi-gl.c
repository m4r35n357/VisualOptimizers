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

    rgb axis_colour = get_colour(DARK_GREY);
    line((gl_point){-10.0F, 0.0F, 0.0F}, (gl_point){10.0F, 0.0F, 0.0F}, axis_colour);
    line((gl_point){0.0F, -10.0F, 0.0F}, (gl_point){0.0F, 10.0F, 0.0F}, axis_colour);
    line((gl_point){0.0F, 0.0F, -10.0F}, (gl_point){0.0F, 0.0F, 10.0F}, axis_colour);

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
            line(v1[i], v1[k], get_colour(DARK_BLUE));
            line(v2[i], v2[k], get_colour(DARK_BLUE));
        }
        ball(v1[i], !i ? get_colour(LIGHT_GREEN) : (i == 3 ? get_colour(LIGHT_RED) : get_colour(LIGHT_GREY)));
        ball(v2[i], !i ? get_colour(LIGHT_GREEN) : (i == 3 ? get_colour(LIGHT_RED) : get_colour(LIGHT_GREY)));
    }

    if (osd_active) {
        sprintf(hud, o.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s1->iterations, s1->evaluations,
                o.places, s1->p[0].x[0], o.places, s1->p[0].x[1], o.places, s1->p[0].x[2], o.places, s1->p[0].f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_YELLOW), hud);
    }

    ReDraw();
}

void CloseWindow () {
    point *best = s1->p[0].f <= s2->p[0].f ? s1->p : s2->p;
    // print solution 1
    fprintf(stderr, "%s%s1%s ", s1->p == best ? "->" : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s1->iterations, s1->evaluations);
    print_point(s1->n, s1->p, o.places, o.fmt);
    // print solution 2
    fprintf(stderr, "%s%s2%s ", s2->p == best ? "->" : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s2->iterations, s2->evaluations);
    print_point(s2->n, s2->p, o.places, o.fmt);
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 9);
    const int n = argc - 6;

    // optimizer settings
    o = get_settings(argv, true);

    // model parameters
    m = model_init();

    // set initial point from command arguments
    point *start = get_point(n);
    for (int j = 0; j < n; j++) {
        start->x[j] = strtod(argv[j + 6], NULL);
    }

    // default simplex . . .
    s1 = get_mds_simplex(n, o.size, start);
    for (int i = 0; i < s1->n + 1; i++) {  // initial cost at simplex vertices
        cost(s1->n, s1->p + i, m);
        s1->evaluations++;
    }
    sort(s1);

    // . . . and its "dual"
    s2 = get_mds_simplex(n, o.size, start);
    for (int i = 0; i < s2->n + 1; i++) {  // form "dual" by projecting vertices through the centre
        project(s2->p + i, s2, m, 1.0L, s2->p + i, start);
    }
    sort(s2);

    // print starting point
    fprintf(stderr, "%s       Initial  ", GRY);
    cost(n, start, m);
    print_point(n, start, o.places, o.fmt);
    fprintf(stderr, o.fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter%s    % .*Lf\n",
            GRY, NRM, o.places, distance(s1->n, s1->p, s1->p + s1->n));

    // get minima for targets if known
    targets = get_known_minima();

    v1 = malloc(4 * sizeof (gl_point)); CHECK(v1);
    get_vertices(v1, s1->p);
    v2 = malloc(4 * sizeof (gl_point)); CHECK(v2);
    get_vertices(v2, s2->p);

    ApplicationInit(argc, argv, "Multidirectional Search Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
