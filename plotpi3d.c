#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pi3d.h>

typedef struct {
    Sphere sphere;
    float pos[3];
    Shader track_shader;
    float vertices[1000][3];
    int track_length;
    int track_max;
    Lines track;
    Lines pointer;
} Body;

void Body_init(Body* self, Shader shader, float colour[3], float radius, float position[3], Shader track_shader, int track_max) {
    Sphere_init(&self->sphere, radius, position[0], position[1], position[2]);
    Sphere_set_draw_details(&self->sphere, shader, NULL);
    self->pos[0] = position[0];
    self->pos[1] = position[1];
    self->pos[2] = position[2];
    self->track_shader = track_shader;
    self->track_length = 0;
    self->track_max = track_max;
    self->track = NULL;
    self->pointer = Lines_init((float[6]){0, 0, 0, 0, 0, 0}, 2);
    self->pointer.vertices[3] = position[0];
    self->pointer.vertices[4] = position[1];
    self->pointer.vertices[5] = position[2];
}

void Body_position_and_draw(Body* self, float trace_material[3]) {
    Sphere_position(&self->sphere, self->pos[0], self->pos[1], self->pos[2]);
    Sphere_draw(&self->sphere);
    self->pointer.vertices[3] = self->pos[0];
    self->pointer.vertices[4] = self->pos[1];
    self->pointer.vertices[5] = self->pos[2];
    Lines_draw(&self->pointer);
    if (self->track_shader) {
        self->track_length += 1;
        self->vertices[self->track_length - 1][0] = self->pos[0];
        self->vertices[self->track_length - 1][1] = self->pos[1];
        self->vertices[self->track_length - 1][2] = self->pos[2];
        self->track = Lines_init(self->vertices, self->track_length);
        Lines_set_shader(&self->track, self->track_shader);
        if (self->track_length > self->track_max) {
            for (int i = 0; i < self->track_max; i++) {
                self->vertices[i][0] = self->vertices[self->track_length - self->track_max + i][0];
                self->vertices[i][1] = self->vertices[self->track_length - self->track_max + i][1];
                self->vertices[i][2] = self->vertices[self->track_length - self->track_max + i][2];
            }
            self->track_length = self->track_max;
        }
        if (self->track) {
            Lines_draw(&self->track);
        }
    }
}

void main() {
    fprintf(stderr, "Multi 3D ODE Plotter: %s\n", argv[0]);
    int argc = argc - 1;
    FILE* files[7];
    if (argc == 0 || argc == 1) {
        files[0] = stdin;
    } else if (argc == 7) {
        for (int i = 0; i < 7; i++) {
            files[i] = fopen(argv[i + 1], "r");
        }
    } else {
        fprintf(stderr, ">>> ERROR! Please supply 1, 2 or 7 arguments! <<<\n");
        exit(1);
    }
    Display* display = Display_create(0, 0, 60);
    Display_set_background(display, 0, 0, 0, 1);
    Camera camera;
    float rot = 135.0;
    float tilt = 90.0 - 54.73561;
    float cam_rad = 50.0;
    Font font = Font_init("/usr/share/fonts/truetype/liberation2/LiberationMono-Regular.ttf", "green", "-0123456789. txyz:=+", 18);
    font.blend = 1;
    String hud = String_init(font, " t 0.0  x 0.0  y 0.0  z 0.0");
    hud.camera.is_3d = 0;
    Shader hud_shader = Shader_init("uv_flat");
    String_set_shader(&hud, hud_shader);
    float hud_bounds[6];
    String_get_bounds(&hud, hud_bounds);
    hud.position[0] = (-display.width + hud_bounds[3] - hud_bounds[0]) / 2.0;
    hud.position[1] = (0.9 * display.height - hud_bounds[4] + hud_bounds[1]) / 2.0;
    hud.position[2] = 1.0;
    String_draw(&hud);
    Body particles[7];
    if (argc == 0) {
        Body_init(&particles[0], Shader_init("mat_light"), (float[3]){0.0, 1.0, 1.0}, 0.1, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), 1000);
    } else if (argc == 1) {
        Body_init(&particles[0], Shader_init("mat_light"), (float[3]){0.0, 1.0, 1.0}, 0.05, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), atoi(argv[1]));
    } else if (argc == 7) {
        Body_init(&particles[0], Shader_init("mat_light"), (float[3]){0.0, 1.0, 1.0}, 0.1, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), 1000);
        Body_init(&particles[1], Shader_init("mat_light"), (float[3]){0.0, 1.0, 1.0}, 0.1, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), 1000);
        Body_init(&particles[2], Shader_init("mat_light"), (float[3]){1.0, 1.0, 0.0}, 0.1, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), 1000);
        Body_init(&particles[3], Shader_init("mat_light"), (float[3]){1.0, 1.0, 0.0}, 0.1, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), 1000);
        Body_init(&particles[4], Shader_init("mat_light"), (float[3]){1.0, 0.0, 1.0}, 0.1, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), 1000);
        Body_init(&particles[5], Shader_init("mat_light"), (float[3]){1.0, 0.0, 1.0}, 0.1, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), 1000);
        Body_init(&particles[6], Shader_init("mat_light"), (float[3]){1.0, 1.0, 1.0}, 0.1, (float[3]){0.0, 0.0, 0.0}, Shader_init("mat_flat"), 1000);
    }
    Keyboard keys = Keyboard_init();
    Mouse mouse = Mouse_init(0);
    Mouse_start(&mouse);
    float omx, omy;
    Mouse_position(&mouse, &omx, &omy);
    while (Display_loop_running(display)) {
        char lines[7][100];
        for (int i = 0; i < argc; i++) {
            fgets(lines[i], 100, files[i]);
        }
        if (!lines[0][0]) {
            Display_stop(display);
        }
        float data[7][4];
        sscanf(lines[0], "%f %f %f %f", &data[0][0], &data[0][1], &data[0][2], &data[0][3]);
        if (argc == 7) {
            for (int i = 0; i < argc; i++) {
                sscanf(lines[i], "%f %f %f %f", &data[i][0], &data[i][1], &data[i][2], &data[i][3]);
            }
        }
        char hud_string[100];
        sprintf(hud_string, " t%.1f  x%.1f  y%.1f  z%.1f", data[0][3], data[0][0], data[0][1], data[0][2]);
        String_quick_change(&hud, hud_string);
        String_draw(&hud);
        Camera_reset(&camera);
        Camera_rotate(&camera, -tilt, rot, 0);
        float r_rot = rot * M_PI / 180.0;
        float r_tilt = tilt * M_PI / 180.0;
        Camera_position(&camera, cam_rad * sin(r_rot) * cos(r_tilt), cam_rad * sin(r_tilt), -cam_rad * cos(r_rot) * cos(r_tilt));
        particles[0].pos[0] = data[0][0];
        particles[0].pos[1] = data[0][1];
        particles[0].pos[2] = data[0][2];
        Body_position_and_draw(&particles[0], (float[3]){0.0, 0.25, 0.0});
        if (argc == 7) {
            particles[1].pos[0] = data[1][0];
            particles[1].pos[1] = data[1][1];
            particles[1].pos[2] = data[1][2];
            Body_position_and_draw(&particles[1], (float[3]){0.0, 0.25, 0.0});
            particles[2].pos[0] = data[2][0];
            particles[2].pos[1] = data[2][1];
            particles[2].pos[2] = data[2][2];
            Body_position_and_draw(&particles[2], (float[3]){0.4, 0.0, 0.0});
            particles[3].pos[0] = data[3][0];
            particles[3].pos[1] = data[3][1];
            particles[3].pos[2] = data[3][2];
            Body_position_and_draw(&particles[3], (float[3]){0.4, 0.0, 0.0});
            particles[4].pos[0] = data[4][0];
            particles[4].pos[1] = data[4][1];
            particles[4].pos[2] = data[4][2];
            Body_position_and_draw(&particles[4], (float[3]){0.0, 0.0, 0.5});
            particles[5].pos[0] = data[5][0];
            particles[5].pos[1] = data[5][1];
            particles[5].pos[2] = data[5][2];
            Body_position_and_draw(&particles[5], (float[3]){0.0, 0.0, 0.5});
            particles[6].pos[0] = data[6][0];
            particles[6].pos[1] = data[6][1];
            particles[6].pos[2] = data[6][2];
            Body_position_and_draw(&particles[6], (float[3]){0.25, 0.25, 0.25});
        }
        float mx, my;
        Mouse_position(&mouse, &mx, &my);
        if (Mouse_button_status(&mouse) == 1) {
            rot -= (mx - omx) * 0.2;
            tilt -= (my - omy) * 0.2;
        } else if (Mouse_button_status(&mouse) == 2) {
            cam_rad += (my - omy) * 0.1;
        }
        omx = mx;
        omy = my;
        int key = Keyboard_read(&keys);
        if (key > -1) {
            if (key == 112) {
                screenshot("trajectory.jpg");
            } else if (key == 27) {
                Keyboard_close(&keys);
                Mouse_stop(&mouse);
                Display_stop(display);
                break;
            }
        }
    }
}
