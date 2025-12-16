#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow *window;
    int width;
    int height;
    const char *title;
} Engine;

int engine_init(Engine *e, int width, int height, const char *title);
void engine_run(Engine *e);
void engine_shutdown(Engine *e);

#endif
