#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "opengl.h"

char hud1[128], hud2[128], hud3[128], hud4[128];

bool initial = true, paused = false, stepping = true, running = true, osd_active = true, centroid = false, minimum = false;

float upper, lower, middle, radius = 10.0F, elevation = 90.0F, azimuth = 0.0F, ball_size = 0.01F;

void SpecialKeyFunc (int Key, int x, int y) { (void)x; (void)y;
    switch (Key) {
        case        GLUT_KEY_UP: elevation += 1.0F; break;
        case      GLUT_KEY_DOWN: elevation -= 1.0F; break;
        case      GLUT_KEY_LEFT: azimuth += 1.0F; break;
        case     GLUT_KEY_RIGHT: azimuth -= 1.0F; break;
        case      GLUT_KEY_HOME: radius -= 0.2F; break;
        case       GLUT_KEY_END: radius += 0.2F; break;
        case   GLUT_KEY_PAGE_UP: radius -= 2.0F; break;
        case GLUT_KEY_PAGE_DOWN: radius += 2.0F; break;
        default: break;
    }
}

void KeyPressFunc (unsigned char Key, int x, int y) { (void)x; (void)y;
    switch (Key) {
        case 'C': case 'c': centroid = !centroid; break;
        case 'M': case 'm': minimum = !minimum; break;
        case 'G': case 'g': ball_size *= 1.1F; break;
        case 'B': case 'b': ball_size /= 1.1F; break;
        case 'S': case 's': running = !running; paused = false; break;
        case 'P': case 'p': stepping = !stepping; paused = false; break;
        case 'F': case 'f': glutFullScreenToggle(); break;
        case 'H': case 'h': osd_active = !osd_active; break;
        case 'Q': case 'q': case 27: exit(0);  // Code 27 is the Escape key
        default: break;
    }
}

void OpenGLInit () {
    glShadeModel(GL_FLAT);
    glClearColor(0.0F, 0.0F, 0.0F, 0.0F);
    glClearDepth(1.0F);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
}

void ResizeWindow (int w, int h) {
    h = !h ? 1 : h;
    w = !w ? 1 : w;
    glViewport(0, 0, w, h);  // View port uses whole window
    glMatrixMode(GL_PROJECTION);  // Set up the projection view matrix (not very well!)
    glLoadIdentity();
    gluPerspective(60.0F, (float)w / (float)h, 1.0F, 3000.0F);
    glMatrixMode(GL_MODELVIEW);  // Select the Modelview matrix
}

void ApplicationInit (int argc, char **argv, char *title) {
    glutInit(&argc, argv);  // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // Need to double buffer for animation
    glutInitWindowPosition(0, 0);  // Create and position the graphics window
    glutInitWindowSize(640, 480);
    glutCreateWindow(title);
    glewInit();  // Initialize GLEW
    OpenGLInit();  // Initialize OpenGL
    glutKeyboardFunc(KeyPressFunc);  // Set up callback functions
    glutSpecialFunc(SpecialKeyFunc);
    glutReshapeFunc(ResizeWindow);
    glutDisplayFunc(Animate);
    fprintf(stderr, "\n   %sOpenGL%s %s\n", GRY, NRM, glGetString(GL_VERSION));
    fprintf(stderr, "%s FreeGLUT%s %d\n", GRY, NRM, glutGet(GLUT_VERSION));
    fprintf(stderr, "     %sGLEW%s %s\n\n", GRY, NRM, glewGetString(GLEW_VERSION));
}

void SetupView () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the rendering window
    glLoadIdentity();
    glTranslatef(0.0F, 0.0F, - radius);
    glRotatef(elevation, 1.0F, 0.0F, 0.0F);
    glRotatef(azimuth, 0.0F, 0.0F, 1.0F);
    glTranslatef(- middle, - middle, - middle);
    glLightfv(GL_LIGHT0, GL_AMBIENT, (float[]){0.0F, 0.0F, 0.0F, 1.0F});
    glLightfv(GL_LIGHT0, GL_DIFFUSE, (float[]){1.0F, 1.0F, 1.0F, 1.0F});
    glLightfv(GL_LIGHT0, GL_SPECULAR, (float[]){1.0F, 1.0F, 1.0F, 1.0F});
    glLightfv(GL_LIGHT0, GL_POSITION, (float[]){-100.0F, 100.0F, -100.0F, 0.0F});
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void ReDraw () {
    glFlush();  // Flush the pipeline, and swap the buffers
    glutSwapBuffers();
    glutPostRedisplay();  // Request a re-draw for animation purposes
}

rgb get_colour (colour_code colour) {
    return (rgb []){
        (rgb){1.0F, 1.0F, 0.0F}, (rgb){0.0F, 1.0F, 1.0F}, (rgb){1.0F, 0.0F, 1.0F},
        (rgb){1.0F, 0.0F, 0.0F}, (rgb){0.0F, 1.0F, 0.0F}, (rgb){0.0F, 0.0F, 1.0F},
        (rgb){0.2F, 0.2F, 0.2F}, (rgb){0.8F, 0.8F, 0.8F}, (rgb){0.5F, 0.5F, 0.5F},
        (rgb){0.5F, 0.5F, 0.0F}, (rgb){0.0F, 0.5F, 0.5F}, (rgb){0.5F, 0.0F, 0.5F},
        (rgb){0.5F, 0.0F, 0.0F}, (rgb){0.0F, 0.5F, 0.0F}, (rgb){0.0F, 0.0F, 0.5F},
        (rgb){1.0F, 1.0F, 1.0F}
    }[colour];
}

void draw_grid () {
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
    line((gl_point){lower, middle, middle}, (gl_point){upper, middle, middle}, axis_colour);
    line((gl_point){middle, lower, middle}, (gl_point){middle, upper, middle}, axis_colour);
    line((gl_point){middle, middle, lower}, (gl_point){middle, middle, upper}, axis_colour);

    line((gl_point){upper, lower, middle}, (gl_point){upper, upper, middle}, axis_colour);
    line((gl_point){upper, middle, lower}, (gl_point){upper, middle, upper}, axis_colour);
    line((gl_point){lower, lower, middle}, (gl_point){lower, upper, middle}, axis_colour);
    line((gl_point){lower, middle, lower}, (gl_point){lower, middle, upper}, axis_colour);

    line((gl_point){lower, upper, middle}, (gl_point){upper, upper, middle}, axis_colour);
    line((gl_point){middle, upper, lower}, (gl_point){middle, upper, upper}, axis_colour);
    line((gl_point){lower, lower, middle}, (gl_point){upper, lower, middle}, axis_colour);
    line((gl_point){middle, lower, lower}, (gl_point){middle, lower, upper}, axis_colour);

    line((gl_point){lower, middle, upper}, (gl_point){upper, middle, upper}, axis_colour);
    line((gl_point){middle, lower, upper}, (gl_point){middle, upper, upper}, axis_colour);
    line((gl_point){lower, middle, lower}, (gl_point){upper, middle, lower}, axis_colour);
    line((gl_point){middle, lower, lower}, (gl_point){middle, upper, lower}, axis_colour);
}

void line (gl_point from, gl_point to, rgb colour) {
    glDisable(GL_LIGHTING);
    glColor3f(colour.a, colour.b, colour.c);
    glBegin(GL_LINES);
    glVertex3f(from.a, from.b, from.c);
    glVertex3f(to.a, to.b, to.c);
    glEnd();
    glEnable(GL_LIGHTING);
}

void ball (gl_point p, rgb colour) {
    glColor3f(colour.a, colour.b, colour.c);
    glPushMatrix();
    glTranslatef(p.a, p.b, p.c);
    glutSolidSphere(ball_size, 10, 10);
    glPopMatrix();
}

void osd_status (char *hud, int fmt, int iterations, int evaluations, int places, point *p) {
    sprintf(hud, fmt ? "%.1d %.1d [ % .*Le % .*Le % .*Le ] % .*Le" : "%.1d %.1d [ % .*Lf % .*Lf % .*Lf ] % .*Lf",
            iterations, evaluations, places, p->x[0], places, p->x[1], places, p->x[2], places, p->f);
}

void osd (int x, int y, rgb colour, char *string) {
    glColor3f(colour.a, colour.b, colour.c);
    glWindowPos2i(x, y);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char *)string);
}
