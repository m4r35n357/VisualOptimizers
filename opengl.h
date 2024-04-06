#pragma once

#include <stdbool.h>
#include <time.h>
#include "base.h"

/*
 * Triple of floats
 */
typedef struct triple_f {
    float a, b, c;
} rgb, gl_point;

typedef enum ColourCode {
    LIGHT_YELLOW=0, LIGHT_CYAN=1, LIGHT_MAGENTA=2, LIGHT_RED=3, LIGHT_GREEN=4, LIGHT_BLUE=5,
    DARK_GREY=6, LIGHT_GREY=7, MID_GREY=8,
    DARK_YELLOW=9, DARK_CYAN=10, DARK_MAGENTA=11, DARK_RED=12, DARK_GREEN=13, DARK_BLUE=14, WHITE=15
} colour_code;

/*
 * Global variables
 */
extern char hud1[], hud2[], hud3[];

extern bool initial, paused, stepping, running, osd_active, centroid, minimum;

extern float upper, lower, centre, radius, ball_size;

/*
 * OpenGL set-up functions
 */
void OpenGLInit (void);

void ApplicationInit (int argc, char **argv, char *title);

/*
 * Callbacks
 */
void SpecialKeyFunc (int Key, int x, int y);

void KeyPressFunc (unsigned char Key, int x, int y);

void ResizeWindow (int w, int h);

void Animate (void);

void CloseWindow (void);

/*
 * View/Camera boilerplate (no logic)
 */
void SetupView (void);

/*
 * Colours
 */
rgb get_colour (colour_code colour);

/*
 * Grid
 */
void draw_grid (void);

/*
 * Lines & balls
 */
void line (gl_point a, gl_point b, rgb colour);

void ball (gl_point p, rgb colour);

/*
 * OSD/HUD
 */
void osd (int x, int y, rgb colour, char *string);

/*
 * Re-draw boilerplate (no logic)
 */
void ReDraw (void);
