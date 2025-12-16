#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>

// Represents a GPU shader program
typedef struct {
    unsigned int id;
} Shader;

// Renderer API
int  renderer_init();
void renderer_draw();
void renderer_shutdown();

// Shader API
Shader shader_load(const char *vertex_path, const char *fragment_path);
void   shader_use(Shader s);
void   shader_destroy(Shader s);

#endif
