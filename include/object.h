#ifndef OBJECT_H
#define OBJECT_H
#include "../include/object.h"

#define MAX_OBJECTS 64
extern int objectCount;
extern int onGround[MAX_OBJECTS + 1];

struct object {
    int index;  

    int vertex_count;
    int triangle_count;
    
    float transformXYZ[3];
    float transformRotation[4];

    float (*points)[3];      // [vertex_count][3]
    int    (*triangles)[3];   // [triangle_count][3]
    float (*triColor)[3];    // [triangle_count][3]
};
extern struct object obj; // declaration

extern struct object objects[MAX_OBJECTS];

void add_object(const struct object *obj, int index);
void strip_whitespace(char *s);
void load_object(const char *filename, struct object *obj);
void free_object(struct object *obj);   
#endif
