#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/camera.h"
#include "../include/object.h"
#include "../include/transform.h"
#include "../include/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>

#define WIDTH 1920
#define HEIGHT 1080

static Shader basic_shader;
static unsigned int VAO, VBO, EBO;
static int index_count = 0;

void draw_object(const struct object *obj){
    
    float *buffer = malloc(sizeof(float) * obj->vertex_count * 6);
    for (int i = 0; i < obj->vertex_count; i++) {
      buffer[i*6+0] = obj->points[i][0];
      buffer[i*6+1] = obj->points[i][1];
      buffer[i*6+2] = obj->points[i][2];
      buffer[i*6+3] = obj->triColor[i][0];
      buffer[i*6+4] = obj->triColor[i][1];
      buffer[i*6+5] = obj->triColor[i][2];
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, obj->vertex_count * 6 * sizeof(float), buffer, GL_STATIC_DRAW);

    
    glBindVertexArray(VAO);
    // Upload vertices
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * obj->triangle_count * 3, obj->triangles, GL_STATIC_DRAW);
    
    // Draw

    glDrawElements(GL_TRIANGLES, obj->triangle_count * 3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0); 
    
    free(buffer);
}


//
// ─────────────────────────────────────────────────────────────
//  Helper: Read a file into a string
// ─────────────────────────────────────────────────────────────
//
static char *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        fprintf(stderr, "Out of memory reading %s\n", path);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);
    return buffer;
}

//
// ─────────────────────────────────────────────────────────────
//  Shader loading
// ─────────────────────────────────────────────────────────────
//
Shader shader_load(const char *vertex_path, const char *fragment_path)
{
    Shader sh;
    sh.id = 0;

    char *vsrc = read_file(vertex_path);
    char *fsrc = read_file(fragment_path);

    if (!vsrc || !fsrc) {
        free(vsrc); free(fsrc);
        return sh;
    }

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, (const char *const *)&vsrc, NULL);
    glCompileShader(vs);

    int success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(vs, 512, NULL, log);
        fprintf(stderr, "Vertex shader error (%s):\n%s\n", vertex_path, log);
    }

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, (const char *const *)&fsrc, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(fs, 512, NULL, log);
        fprintf(stderr, "Fragment shader error (%s):\n%s\n", fragment_path, log);
    }

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(prog, 512, NULL, log);
        fprintf(stderr, "Shader linking error:\n%s\n", log);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    free(vsrc);
    free(fsrc);

    sh.id = prog;
    return sh;
}

void shader_use(Shader s)
{
    glUseProgram(s.id);
}

void shader_destroy(Shader s)
{
    glDeleteProgram(s.id);
}

//  Renderer initialization

int renderer_init()
{
    // Must load GL after context exists
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 0;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    // Load shaders
    basic_shader = shader_load(
        "../shaders/basic.vert",
        "../shaders/basic.frag"
    );
    if (basic_shader.id == 0) {
        fprintf(stderr, "Failed to load basic shader\n");
        return 0;
    }

    
    // Setup VAO/VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);  

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Background color
    glClearColor(sin(glfwGetTime()), 0.0666666f, 0.10588f, 1.0f);

    return 1;
}

//
// ─────────────────────────────────────────────────────────────
//  Draw frame
// ─────────────────────────────────────────────────────────────
//
mat4 projection, view, model;

void renderer_draw(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_use(basic_shader);
    glEnable(GL_DEPTH_TEST);

    shader_use(basic_shader);
    
    mat4 model;
    glm_mat4_identity(model);
    unsigned int modelLoc = glGetUniformLocation(basic_shader.id, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);  
    
    mat4 *view = camera_get_view(&camera);
    mat4 *proj = camera_get_proj(&camera);
    
    unsigned int viewLoc = glGetUniformLocation(basic_shader.id, "uView");
    unsigned int projLoc = glGetUniformLocation(basic_shader.id, "uProj");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)proj);

    
    for (int i = 0; i < objectCount; i++) {
      draw_object(&objects[i]);
    }
    glClearColor((cos(glfwGetTime()) + 1 )/ 2, (cos(glfwGetTime() - (2 * M_PI) / 3) + 1) / 2, (sin(glfwGetTime() + ( 2 * M_PI / 3)) + 1) / 2, 1.0f);
}

//
// ─────────────────────────────────────────────────────────────
//  Cleanup
// ─────────────────────────────────────────────────────────────
//
void renderer_shutdown()
{
    shader_destroy(basic_shader);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
