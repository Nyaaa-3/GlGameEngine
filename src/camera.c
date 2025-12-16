#include "../include/transform.h"
#include "../include/collitions.h"
#include "../include/camera.h"
#include <cglm/cam.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SENSITIVITY 0.03125

Camera camera;

double lastxpos = 0, lastypos = 0;
double xpos, ypos;

void inputRotation(GLFWwindow *window){

  glfwGetCursorPos(window, &xpos, &ypos);
  camera.yaw += -(lastxpos - xpos) * SENSITIVITY;
  camera.pitch += (lastypos - ypos) * SENSITIVITY;

  lastxpos = xpos;
  lastypos = ypos;
}
vec3 velocity_xz = {0.0f, 0.0f, 0.0f};

void AirControl(vec3 wishDir, float deltaTime)
{
    float speed = glm_vec3_norm(velocity_xz);
    if (speed < 0.01f)
        return;

    float dot = glm_vec3_dot(velocity_xz, wishDir);

    /* If player is steering against velocity, allow braking */
    if (dot < 0.0f) {
        vec3 brake;
        glm_vec3_scale(wishDir, dot * 6.0f * deltaTime, brake);
        glm_vec3_sub(velocity_xz, brake, velocity_xz);
    }
}

void Accelerate(vec3 wishDir, float wishSpeed, float accel, float deltaTime)
{
    float currentSpeed = glm_vec3_dot(velocity_xz, wishDir);
    float addSpeed = wishSpeed - currentSpeed;

    if (addSpeed <= 0.0f)
        return;

    float accelSpeed = accel * wishSpeed * deltaTime;
    if (accelSpeed > addSpeed)
        accelSpeed = addSpeed;
    
  

    vec3 accelVec;
    glm_vec3_scale(wishDir, accelSpeed, accelVec);
    glm_vec3_add(velocity_xz, accelVec, velocity_xz);
}


void ApplyFriction(float deltaTime)
{
    float speed = glm_vec3_norm(velocity_xz);
    if (speed < 0.001f)
        return;

    float drop = speed * 20.0f * deltaTime; // friction strength
    float newSpeed = speed - drop;
    if (newSpeed < 0)
        newSpeed = 0;

    glm_vec3_scale(velocity_xz, newSpeed / speed, velocity_xz);
}


float cooldown;

void inputXYZ(Camera *cam, GLFWwindow *window, float deltaTime){
    vec3 fwd = { cam->front[0], 0.0f, cam->front[2] };
    vec3 right = { cam->right[0], 0.0f, cam->right[2] };
    glm_normalize(fwd);
    glm_normalize(right);

    vec3 wishDir = GLM_VEC3_ZERO_INIT;
    float wishSpeed = 12.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
      glm_vec3_add(wishDir, fwd, wishDir);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        glm_vec3_sub(wishDir, fwd, wishDir);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        glm_vec3_add(wishDir, right, wishDir);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        glm_vec3_sub(wishDir, right, wishDir);

    if (glm_vec3_norm(wishDir) > 0.0f)
        glm_normalize(wishDir);

    if (onGround[MAX_OBJECTS]) {
        ApplyFriction(deltaTime);
        AirControl(wishDir, deltaTime);
        Accelerate(wishDir, wishSpeed, 12.0f, deltaTime);
    } else {
        Accelerate(wishDir, wishSpeed, 2.5f, deltaTime); // air control
    }

    /* Dash = velocity impulse */
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && cooldown <= 0.0f) {
        vec3 dash;
        glm_vec3_scale(wishDir, 12.0f, dash);
        glm_vec3_add(velocity_xz, dash, velocity_xz);

        cooldown = 0.8f;
    }

    /* Apply horizontal velocity */
    glm_vec3_muladds(velocity_xz, deltaTime, cam->position);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        CamJump(4.2f);

    if (cooldown > 0.0f)
        cooldown -= deltaTime;
}


void camera_init(Camera *cam, float aspect){
    glm_vec3_copy((vec3){0.0f, 10.0f, 0.0f}, cam->position);

    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, cam->front);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, cam->world_up);

    cam->yaw   = -90.0f;
    cam->pitch = 0.0f;

    cam->fov = 100.0f;
    cam->aspect = aspect;
    cam->near_plane = 0.1f;
    cam->far_plane  = 100.0f;

    //camera_update(cam);
}
void camera_update(Camera *cam, GLFWwindow *window, double deltatime){
    // update direction (front)
  
    camDefineBounds(cam->position);

    CamGravity(cam->position, 0.0, deltatime);
    
    inputRotation(window);
    //printf("%f\n", deltatime);
    //printf("%d\n",camColliding(1));
    
    

    vec3 front;
    front[0] = cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    front[1] = sinf(glm_rad(cam->pitch));
    front[2] = sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    glm_vec3_normalize_to(front, cam->front);

    // update right and up
    glm_vec3_cross(cam->front, cam->world_up, cam->right);
    glm_vec3_normalize(cam->right);

    glm_vec3_cross(cam->right, cam->front, cam->up);
    glm_vec3_normalize(cam->up);

    inputXYZ(cam, window, deltatime);
    
    xzcolliding(cam->position);
    
    // view matrix
    glm_look(cam->position, cam->front, cam->up, cam->view);

    // projection matrix
    glm_perspective(glm_rad(cam->fov), cam->aspect, cam->near_plane,
                    cam->far_plane, cam->proj);
}

mat4 *camera_get_view(Camera *cam) { return &cam->view; }
mat4 *camera_get_proj(Camera *cam) { return &cam->proj; }
