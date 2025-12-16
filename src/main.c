#include "../include/transform.h"
#include "../include/engine.h"
#include "../include/object.h"
#include "../include/renderer.h"
#define WIDTH 1920 
#define HEIGHT 1080

int main(){

    Engine engine;
    
    if (!engine_init(&engine, WIDTH, HEIGHT, "My OpenGL Game")) {
        return -1;
    }

    engine_run(&engine);
    engine_shutdown(&engine);

    return 0;
}
