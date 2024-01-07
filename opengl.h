/*
 * (c) 2018-2023 m4r35n357@gmail.com (Ian Smith), for licencing see the LICENCE file
 */

#pragma once

#include <stdbool.h>
#include <time.h>

/*
 * Triple of floats
 */
typedef struct triple_f {
    float a, b, c;
} rgb, vertex;

typedef enum ColourCode {LIGHT_YELLOW=0, LIGHT_CYAN=1, LIGHT_MAGENTA=2, LIGHT_RED=3, LIGHT_GREEN=4, LIGHT_BLUE=5,
						DARK_GREY=6, LIGHT_GREY=7, GREY=8} colour_code;

/*
 * Global variables
 */
extern char hud[];

extern clock_t since;

extern bool finished, paused, stepping, running, osd_active, solid, axes, centroid;

extern int colour_index, mesh;

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
 * Lines & balls
 */
void line (vertex a, vertex b, rgb colour);

void point_position (vertex p, rgb colour, float scale);

/*
 * OSD/HUD
 */
void osd (int x, int y, char *string);

/*
 * Extract current coordinates from data
 */
vertex get_current (void *data);

/*
 * Push latest point to the track buffer
 */
void buffer_point (void);

/*
 * Re-draw boilerplate (no logic)
 */
void ReDraw (void);
