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
static optimset opt;
static point *solution;
static gl_point *vertices;

gl_point get_gl_point (point *p) {
    return (gl_point){(float)p->x[0], (float)p->x[1], (float)p->x[2]};
}

void Animate () {
    SetupView();

    if (axes) {
        rgb axis_colour = get_colour(DARK_GREY);
        line((gl_point){-10.0F, 0.0F, 0.0F}, (gl_point){10.0F, 0.0F, 0.0F}, axis_colour);
        line((gl_point){0.0F, -10.0F, 0.0F}, (gl_point){0.0F, 10.0F, 0.0F}, axis_colour);
        line((gl_point){0.0F, 0.0F, -10.0F}, (gl_point){0.0F, 0.0F, 10.0F}, axis_colour);
    }

    if (centroid) {
        gl_point c = get_gl_point(s->centre);
        for (int i = 0; i < 4; i++) {
            line(c, vertices[i], get_colour(DARK_GREY));
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int k = i; k < 4; k++) {
            line(vertices[i], vertices[k], get_colour(GREY));
        }
        rgb vertex_colour;
        switch (i) {
            case  0: vertex_colour = get_colour(LIGHT_GREEN); break;
            case  2: vertex_colour = get_colour(LIGHT_YELLOW); break;
            case  3: vertex_colour = get_colour(LIGHT_RED); break;
            default: vertex_colour = get_colour(LIGHT_GREY); break;
        }
        point_position(vertices[i], vertex_colour, 1.0F);
    }

    if (osd_active) {
        glColor3f(0.0F, 0.5F, 0.5F);
        sprintf(hud, opt.fmt ? "%.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
                s->iterations, opt.precision, s->p[0].x[0], opt.precision, s->p[0].x[1], opt.precision, s->p[0].x[2], opt.precision, s->p[0].f);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, hud);
    }

    if (!finished && !paused) {
        if (nelder_mead(s, solution, m, &opt)) {
            for (int i = 0; i < 4; i++) {
                vertices[i] = get_gl_point(s->p + i);
            }
        } else finished = true;
        if (stepping) paused = true;
    }

    ReDraw();
}

void CloseWindow () {
}

int main (int argc, char **argv) {
    since = clock();

    PRINT_ARGS(argc, argv);
    CHECK(argc >= 12);

    // optimizer settings
    opt = get_settings(argv);

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
    s = get_simplex(n, opt.size, start);
    s->gl = true;

    // print starting point
    printf("%s     Initial ", GRY);
    cost(n, start, m);
    print_point(n, start, opt.precision, opt.fmt);

    vertices = malloc(4 * sizeof (gl_point)); CHECK(vertices);

    ApplicationInit(argc, argv, "Nelder-Mead Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.
    return 0 ;          // Compiler requires this to be here. (Never reached)
}
