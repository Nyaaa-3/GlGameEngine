#include "../include/transform.h"
#include "../include/camera.h"
#include "../include/engine.h"
#include "../include/object.h"
#include "../include/renderer.h"
#include "../include/collitions.h"
#include <stdio.h>
#include <time.h>

int levelChange = 1, levelIndex = 1;

struct object obj;
struct object cubw;
struct object cubx;
struct object ground;
struct object objects[MAX_OBJECTS];

int levelLoad(){
  getBoxes(objects);
}

void object_init(){
  
  switch(levelIndex){
    default:
    
    break;
    case 1:
      obj.transformRotation[0] = 0.01;
      obj.transformRotation[1] = 0;
      obj.transformRotation[2] = 1;
      obj.transformRotation[3] = 0;
    
      obj.transformXYZ[0] = 0;
      obj.transformXYZ[1] = 0;
      obj.transformXYZ[2] = 0;
    
      cubw.transformXYZ[0] = 2;
      cubw.transformXYZ[1] = 0;
      cubw.transformXYZ[2] = 2;
    
      cubx.transformXYZ[0] = -2;
      cubx.transformXYZ[1] = 0;
      cubx.transformXYZ[2] = -2;
      
      load_object("../assets/level1/cube.3d", &obj);
      load_object("../assets/level1/cubw.3d", &cubw);
      load_object("../assets/level1/cubx.3d", &cubx);
      load_object("../assets/level1/floor.3d", &ground); 

      add_object(&obj, 0);
      add_object(&ground, 1);
      add_object(&cubw, 2);
      add_object(&cubx, 3);
      
      transformXYZ(obj.vertex_count, obj.points, obj.transformXYZ, obj.index);
      transformXYZ(cubw.vertex_count, cubw.points, cubw.transformXYZ, cubw.index);
      transformXYZ(cubx.vertex_count, cubx.points, cubx.transformXYZ, cubx.index);
    break;
  }


}

int engine_init(Engine *e, int width, int height, const char *title){
    
    object_init();

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    e->width = width;
    e->height = height;
    e->title = title;

    e->window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!e->window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return 0;
    }

    camera_init(&camera, (float)width / height);
    
    glfwMakeContextCurrent(e->window);

    // Enable vsync (you can disable later)
    glfwSwapInterval(1);

    // Initialize OpenGL loader + shaders
    if (!renderer_init(&obj)) {
        fprintf(stderr, "Renderer initialization failed.\n");
        glfwDestroyWindow(e->window);
        glfwTerminate();
        return 0;
    }
}

void engine_run(Engine *e){
    //hijinks  
  const double target_frametime = 0.0;// 1.0 / 6666666666666666.0;   // 60 FPS cap
  struct timespec frame_start;
  clock_gettime(CLOCK_MONOTONIC, &frame_start);
  double lastTime = glfwGetTime();
  
  extern Camera camera;

  glfwSetInputMode(e->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
  
  while (!glfwWindowShouldClose(e->window)) {

    double current = glfwGetTime();
    double deltatime = current - lastTime;
    lastTime = current;

    struct timespec now;
    
    clock_gettime(CLOCK_MONOTONIC, &now);

    double elapsed = (now.tv_sec - frame_start.tv_sec)
                   + (now.tv_nsec - frame_start.tv_nsec) / 1e9;

    double remaining = target_frametime - elapsed;

    if (remaining > 0) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = (long)(remaining * 1e9);
        nanosleep(&ts, NULL);
    }        
        
    if(levelChange == 1){
      levelLoad();
      levelChange = 0;
    }
    
    for(int i = 0; i < objectCount; i++){
      if(i != 1 && i != 3){
        Gravity(objects[i].vertex_count, objects[i].points, -1, deltatime, objects[i].index);
      }
    }


    //TransformRotation(obj.vertex_count, obj.points, obj.transformRotation);
    glfwPollEvents();
    // printf("%.9f\n", 1 / deltatime);
        
    camera_update(&camera, e->window, deltatime);
        // draw frame
    renderer_draw();
      
        // present
    glfwSwapBuffers(e->window);

        // handle input/events
    clock_gettime(CLOCK_MONOTONIC, &frame_start);
  }
}

void engine_shutdown(Engine *e)
{
    renderer_shutdown();  // clean OpenGL state

    glfwDestroyWindow(e->window);
    glfwTerminate();
}
