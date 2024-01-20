#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"

static simplex *s;
static model *m;
static minima *targets;
static optimset o;
static point *solution;
static gl_point *v;

static gl_point get_gl_point (real *point) {
    return (gl_point){(float)point[0], (float)point[1], (float)point[2]};
}

void get_vertices (gl_point *vertices, point *points) {
    for (int i = 0; i < 4; i++) {
        vertices[i] = get_gl_point(points[i].x);
    }
}

void Animate () {
    SetupView();

    if (!finished && !paused) {
        if (initial) {
            initial = false;
        } else {
            if (nelder_mead(s, solution, m, &o)) {
                get_vertices(v, s->p);
            } else finished = true;
        }
        if (stepping) paused = true;
    }

    rgb axis_colour = get_colour(DARK_GREY);
    line((gl_point){-10.0F, 0.0F, 0.0F}, (gl_point){10.0F, 0.0F, 0.0F}, axis_colour);
    line((gl_point){0.0F, -10.0F, 0.0F}, (gl_point){0.0F, 10.0F, 0.0F}, axis_colour);
    line((gl_point){0.0F, 0.0F, -10.0F}, (gl_point){0.0F, 0.0F, 10.0F}, axis_colour);

    if (centroid) {
        for (int i = 0; i < 4; i++) {
            line(get_gl_point(s->centroid->x), v[i], i == 3 ? get_colour(DARK_RED) : get_colour(DARK_BLUE));
        }
    }

    if (targets) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(LIGHT_YELLOW));
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int k = i; k < 4; k++) {
            line(v[i], v[k], !i ? get_colour(DARK_GREEN) : get_colour(DARK_CYAN));
        }
        ball(v[i], !i ? get_colour(LIGHT_GREEN) : (i == 3 ? get_colour(LIGHT_RED) : get_colour(LIGHT_GREY)));
    }

    if (osd_active) {
        sprintf(hud, o.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s->iterations, s->evaluations,
                o.places, s->p[0].x[0], o.places, s->p[0].x[1], o.places, s->p[0].x[2], o.places, s->p[0].f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_YELLOW), hud);
    }

    ReDraw();
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 10);

    // optimizer settings
    o = get_settings(argv);

    const int n = argc - 7;
    point *start = get_point(n);
    // set initial point from command arguments
    for (int j = 0; j < n; j++) {
        start->x[j] = strtod(argv[j + 7], NULL);
    }
    solution = get_point(n);

    // model parameters
    m = model_init();

    // get a simplex
    s = get_simplex(n, o.size, start, m);
    s->gl = true;

    // get minima for targets if known
    targets = get_known_minima();

    // print starting point
    printf("%s     Initial ", GRY);
    cost(n, start, m);
    print_point(n, start, o.places, o.fmt);

    v = malloc(4 * sizeof (gl_point)); CHECK(v);
    get_vertices(v, s->p);

    ApplicationInit(argc, argv, "Nelder-Mead Visualizer");
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.
    return 0 ;          // Compiler requires this to be here. (Never reached)
}
