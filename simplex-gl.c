#include <math.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "simplex.h"

static simplex *s1, *s2;
static model *m;
static minima *targets;
static optimset o;
static gl_point *v1, *v2;

static void get_vertices (gl_point *vertices, point *points) {
    for (int i = 0; i < (o.n + 1); i++) {
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

    draw_grid();

    if (centroid) {
        for (int i = 0; i < o.n; i++) {
            line(get_gl_point(s1->centroid->x), v1[i], get_colour(DARK_MAGENTA));
            line(get_gl_point(s2->centroid->x), v2[i], get_colour(DARK_MAGENTA));
        }
    }

    if (targets && minimum) {
        for (int i = 0; i < targets->n_minima; i++) {
            ball(get_gl_point(targets->min[i].x), get_colour(WHITE));
        }
    }

    for (int i = 0; i < (o.n + 1); i++) {
        for (int k = i; k < (o.n + 1); k++) {
            line(v1[i], v1[k], get_colour(DARK_GREY));
            line(v2[i], v2[k], get_colour(DARK_GREY));
        }
        ball(v1[i], !i ? get_colour(LIGHT_GREEN) : (i == o.n ? get_colour(LIGHT_RED) : get_colour(DARK_CYAN)));
        ball(v2[i], !i ? get_colour(LIGHT_GREEN) : (i == o.n ? get_colour(LIGHT_RED) : get_colour(DARK_YELLOW)));
    }

    if (osd_active) {
        osd_status(hud1, o.fmt, s1->iterations, s1->evaluations, o.places, s1->p);
        osd_status(hud2, o.fmt, s2->iterations, s2->evaluations, o.places, s2->p);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, get_colour(DARK_CYAN), hud1);
        osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 40, get_colour(DARK_YELLOW), hud2);
        if (targets && minimum) {
            osd_status(hud3, o.fmt, 0, 0, o.places, targets->min);
            osd(10, glutGet(GLUT_WINDOW_HEIGHT) - 60, get_colour(LIGHT_GREY), hud3);
        }
    }

    ReDraw();
}

void CloseWindow () {
    point *best = s1->p[0].f <= s2->p[0].f ? s1->p : s2->p;
    // print solution 1
    fprintf(stderr, "%s%s%sDefault%s ", WHT, s1->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s1->iterations, s1->evaluations);
    print_result(o.n, s1->p, o.places, o.fmt);
    // print solution 2
    fprintf(stderr, "%s%s%s   Dual%s ", WHT, s2->p == best ? "* " : "  ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s2->iterations, s2->evaluations);
    print_result(o.n, s2->p, o.places, o.fmt);
}

int main (int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 8);

    // optimizer settings
    o = get_settings(argv);

    // model parameters
    m = model_init(o.n);

    point *centre = get_point(o.n);
    if (o.init_mode == 0) {
        CHECK(argc == 8 + o.n);
        real max = 0.0L;
        for (int j = 0; j < o.n; j++) {
            centre->x[j] = strtold(argv[8 + j], NULL);
            if (fabsl(centre->x[j]) > max) max = fabsl(centre->x[j]);
        }
        o.upper = 2.0L * max;
        o.lower = -o.upper;
    } else {
        CHECK(argc == 10);
        o.lower = strtold(argv[8], NULL);
        o.upper = strtold(argv[9], NULL);  CHECK(o.upper > o.lower);
        set_random_coordinates(centre, o.n, o.lower, o.upper);
    }
    cost(o.n, centre, m);

    // default simplex . . .
    s1 = get_simplex(o.n, o.size, centre, m, o.adaptive);

    // . . . and its "dual"
    s2 = get_simplex(o.n, o.size, centre, m, o.adaptive);
    for (int i = 0; i < o.n + 1; i++) {  // form "dual" by projecting vertices through the centre
        project(s2->p + i, s2, o.n, m, 1.0L, s2->p + i, centre);
    }
    sort(s2, o.n);

    // print starting point
    fprintf(stderr, "%s       Initial  ", GRY);
    print_result(o.n, centre, o.places, o.fmt);
    fprintf(stderr, o.fmt ? "      %sDiameter%s% .*Le\n" : "      %sDiameter%s% .*Lf\n",
            GRY, NRM, o.places, distance(o.n, s1->p, s1->p + o.n));

    // get minima for targets if known
    targets = get_known_minima(o.n);

    size_t gl_simplex = (size_t)(o.n + 1) * sizeof (gl_point);
    v1 = malloc(gl_simplex); CHECK(v1);
    v2 = malloc(gl_simplex); CHECK(v2);
    get_vertices(v1, s1->p);
    get_vertices(v2, s2->p);

    lower = (float)o.lower;
    upper = (float)o.upper;
    middle = (float)(0.5L * (o.lower + o.upper));
    radius = 1.5F * ((float)o.upper - (float)o.lower);
    ball_size = 0.002F * ((float)o.upper - (float)o.lower);

    ApplicationInit(argc, argv, "Nelder-Mead Visualizer");
    glutCloseFunc(CloseWindow);
    glutMainLoop();     // Start the main loop.  glutMainLoop never returns.

    return 0 ;          // Compiler requires this to be here. (Never reached)
}
