/*
 *  OpenGL common code
 *
 * (c) 2018-2023 m4r35n357@gmail.com (Ian Smith), for licencing see the LICENCE file
 */

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "opengl.h"
#include "nelder_mead.h"

char hud[128];

clock_t since;

bool finished = false, paused = false, stepping = true, running = true, osd_active = true, solid = true, axes = true, centroid = false;

int colour_index = 13, mesh = 10;

static float radius = 5.0F, latitude = 90.0F, longitude = 0.0F, ball_size = 0.01F;

void SpecialKeyFunc (int Key, int x, int y) { (void)x; (void)y;
    switch (Key) {
        case        GLUT_KEY_UP: latitude += 1.0F; break;
        case      GLUT_KEY_DOWN: latitude -= 1.0F; break;
        case      GLUT_KEY_LEFT: longitude += 1.0F; break;
        case     GLUT_KEY_RIGHT: longitude -= 1.0F; break;
        case      GLUT_KEY_HOME: radius -= 0.2F; break;
        case       GLUT_KEY_END: radius += 0.2F; break;
        case    GLUT_KEY_INSERT: solid = !solid; break;
        case   GLUT_KEY_PAGE_UP: mesh++; break;
        case GLUT_KEY_PAGE_DOWN: mesh = mesh > 2 ? mesh - 1 : mesh; break;
        default: break;
    }
}

void KeyPressFunc (unsigned char Key, int x, int y) { (void)x; (void)y;
    switch (Key) {
        case 'A': case 'a': axes = !axes; break;
        case 'C': case 'c': centroid = !centroid; break;
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
    gluPerspective(60.0F, (float)w / (float)h, 1.0F, 100.0F);
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
    glRotatef(latitude, 1.0F, 0.0F, 0.0F);
    glRotatef(longitude, 0.0F, 0.0F, 1.0F);
    glLightfv(GL_LIGHT0, GL_POSITION, (float []){-100.0F, 100.0F, -100.0F, 0.0F});
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
        (rgb){0.5F, 0.0F, 0.0F}, (rgb){0.0F, 0.5F, 0.0F}, (rgb){0.0F, 0.0F, 0.5F}
    }[colour];
}

void line (vertex from, vertex to, rgb colour) {
    glColor3f(colour.a, colour.b, colour.c);
    glBegin(GL_LINES);
    glVertex3f(from.a, from.b, from.c);
    glVertex3f(to.a, to.b, to.c);
    glEnd();
}

void point_position (vertex p, rgb colour, float scale) {
    glColor3f(colour.a, colour.b, colour.c);
    glPushMatrix();
    glTranslatef(p.a, p.b, p.c);
    solid ? glutSolidSphere(ball_size * scale, mesh, mesh) : glutWireSphere(ball_size * scale, mesh, mesh);
    glPopMatrix();
}

void osd (int x, int y, char *string) {
    glWindowPos2i(x, y);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char *)string);
}
