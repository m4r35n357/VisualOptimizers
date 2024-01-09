/*
 *  Nelder-Mead OpenGL display
 *
 * (c) 2018-2024 m4r35n357@gmail.com (Ian Smith), for licencing see the LICENCE file
 */

#include <stdio.h>
#include <GL/freeglut.h>
#include "opengl.h"

static simplex *s;  // the simplex
static model *m;  // the model
static optimset o;
static point *solution;
static gl_point *v;

void get_vertices (gl_point *vertices, point *points) {
    for (int i = 0; i < 4; i++) {
        vertices[i] = (gl_point){(float)points[i].x[0], (float)points[i].x[1], (float)points[i].x[2]};
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

    for (int i = 0; i < 4; i++) {
        for (int k = i; k < 4; k++) {
            line(v[i], v[k], get_colour(DARK_BLUE));
        }
        rgb vertex_colour;
        switch (i) {
            case  0: vertex_colour = get_colour(LIGHT_GREEN); break;
            case  2: vertex_colour = get_colour(LIGHT_YELLOW); break;
            case  3: vertex_colour = get_colour(LIGHT_RED); break;
            default: vertex_colour = get_colour(LIGHT_GREY); break;
        }
        ball(v[i], vertex_colour);
    }

    if (osd_active) {
        glColor3f(0.0F, 0.5F, 0.5F);
        sprintf(hud, o.fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s->iterations, s->evaluations,
                o.places, s->p[0].x[0], o.places, s->p[0].x[1], o.places, s->p[0].x[2], o.places, s->p[0].f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, hud);
    }

    ReDraw();
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 12);

    // optimizer settings
    o = get_settings(argv);

    const int n = argc - 10;
    point *start = get_point(n);
    // set initial point from command arguments
    for (int j = 0; j < n; j++) {
        start->x[j] = strtod(argv[j + 10], NULL);
    }
    solution = get_point(n);

    // model parameters
    m = get_parameters();

    // get a simplex
    s = get_simplex(n, o.size, start);
    s->gl = true;

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
