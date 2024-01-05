/*
 *  N-Body OpenGL display
 *
 * (c) 2018-2023 m4r35n357@gmail.com (Ian Smith), for licencing see the LICENCE file
 */

#include <stdio.h>
#include <GL/freeglut.h>
#include "nelder_mead.h"
#include "opengl.h"

static simplex *s;  // the simplex
static model *m;  // the model

point_gl get_current (void *data) {
	point *_ = (point *)data;
    return (point_gl){(float)_->x[0], (float)_->x[1], (float)_->x[2]};
}

void Animate () {
    SetupView();

    if (mode == BOTH || mode == POSITION) {
        //for (int j = 0; j < nb->n; j++) {
        //    line_position(t[j].points[newest], t[j].colour, nb->bodies[j].r);
        //}
    }

    if (osd_active) {
        glColor3f(0.0F, 0.5F, 0.5F);
        //sprintf(hud, "t: %.1Lf  h: %.6Le  ~sf: %.1Lf", c->step * c->h, h, error(h - nb->h0));
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, hud);
        osd_summary();
    }

    if (!finished && !paused) {
        if (nelder_mead(s, solution, m, &opt)) {
            for (int j = 0; j < s->n; j++) {
                s->p[j] = get_current_point(s->p[j]);
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
    optimset opt = {
        .precision = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .debug = (int)strtol(argv[3], NULL, BASE),
        .x_tolerance = strtold(argv[4], NULL),
        .f_tolerance = strtold(argv[5], NULL),
        .max_iterations = (int)strtol(argv[6], NULL, BASE),
        .max_evaluations = (int)strtol(argv[7], NULL, BASE),
        .adaptive = (int)strtol(argv[8], NULL, BASE),
        .size = strtold(argv[9], NULL)
    };
    CHECK(opt.precision >= 3 && opt.precision <= 36);
    CHECK(opt.debug == 0 || opt.debug == 1);
    CHECK(opt.x_tolerance >= 1.0e-36L && opt.x_tolerance <= 1.0e-3L);
    CHECK(opt.f_tolerance >= 1.0e-36L && opt.f_tolerance <= 1.0e-3L);
    CHECK(opt.max_iterations >= 1 && opt.max_iterations <= 100000);
    CHECK(opt.max_evaluations >= 1 && opt.max_evaluations <= 100000);
    CHECK(opt.adaptive == 0 || opt.adaptive == 1);
    CHECK(opt.size >= 1.0e-12L && opt.size <= 1.0e3L);

    const int n = argc - 10;
    point *start = get_point(n);
    // set initial point from command arguments
    for (int i = 0; i < n; i++) {
        start->x[i] = strtod(argv[i + 10], NULL);
    }
    point *solution = get_point(n);

    // model parameters
    *m = get_parameters();

    // get a simplex
    *s = get_simplex(n, opt.size, start);

    // begin optimization
    nelder_mead(s, solution, m, &opt);

    // print starting point
    printf("%s     Initial ", GRY);
    cost(n, start, m);
    print_point(n, start, opt.precision, opt.fmt);
    // print solution
    printf("    %sSolution ", GRY);
    print_point(n, solution, opt.precision, opt.fmt);
    // print stats
    printf("%s  Iterations/Evaluations%s %d/%d\n", GRY, NRM, s->iterations, s->evaluations);

    ApplicationInit(argc, argv, "N-Body Plotter");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.
    return 0 ;          // Compiler requires this to be here. (Never reached)
}
