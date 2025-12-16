
#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct Camera {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    
    int onGround;
    
    float yaw;
    float pitch;

    float fov;
    float aspect;
    float near_plane;
    float far_plane;

    mat4 view;
    mat4 proj;

} Camera;

void camera_init(Camera *cam, float aspect);
void camera_update(Camera *cam, GLFWwindow *window, double deltatime);
void camera_set_position(Camera *cam, float x, float y, float z);
void camera_set_rotation(Camera *cam, float yaw_deg, float pitch_deg);

// these provide matrices to renderer
mat4 *camera_get_view(Camera *cam);
mat4 *camera_get_proj(Camera *cam);

extern Camera camera;

#endif
