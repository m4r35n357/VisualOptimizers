#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "whale.h"

static population *p;
static model *m;
static minima *targets;
static options o;
static point *solution;
static gl_point *v;
static real minx;
static real maxx;

static gl_point get_gl_point (real *point) {
    return (gl_point){(float)point[0], (float)point[1], (float)point[2]};
}

static void get_vertices (gl_point *vertices, whale **whales) {
    for (int i = 0; i < o.whales; i++) {
        vertices[i] = get_gl_point(whales[i]->x);
    }
}

void Animate () {
    SetupView();

    if (!paused) {
        if (initial) {
            initial = false;
        } else {
            if (!finished1) {
                woa(p, solution, minx, maxx, m, o);
                get_vertices(v, p->whales);
            }
        }
        if (stepping) paused = true;
    }

    rgb axis_colour = get_colour(DARK_GREY);
    line((gl_point){-10.0F, 0.0F, 0.0F}, (gl_point){10.0F, 0.0F, 0.0F}, axis_colour);
    line((gl_point){0.0F, -10.0F, 0.0F}, (gl_point){0.0F, 10.0F, 0.0F}, axis_colour);
    line((gl_point){0.0F, 0.0F, -10.0F}, (gl_point){0.0F, 0.0F, 10.0F}, axis_colour);

    if (targets) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(LIGHT_YELLOW));
        }
    }

    for (int i = 0; i < o.whales; i++) {
        ball(v[i], get_colour(LIGHT_GREEN));
    }

    if (osd_active) {
//        sprintf(hud, o.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
//                s1->iterations, s1->evaluations,
//                o.places, s1->p[0].x[0], o.places, s1->p[0].x[1], o.places, s1->p[0].x[2], o.places, s1->p[0].f);
//        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_YELLOW), hud);
    }

    ReDraw();
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc == 8);

    // options
    o = get_options(argv, true);
    minx = strtold(argv[6], NULL);
    maxx = strtold(argv[7], NULL);

    // model parameters
    m = model_init();

    p = get_population(minx, maxx, m, o);

    solution = get_whale(o.dim, minx, maxx, m);
    woa(p, solution, minx, maxx, m, o);

    // get minima for targets if known
    targets = get_known_minima();

    v = malloc((size_t)o.whales * sizeof (gl_point)); CHECK(v);
    get_vertices(v, p->whales);

    ApplicationInit(argc, argv, "Nelder-Mead Visualizer");
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.
    return 0 ;          // Compiler requires this to be here. (Never reached)
}
