#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/object.h"


//int object_count = 2;
//struct object objects[MAX_OBJECTS];
int objectCount;

void add_object(const struct object *obj, int index) {
    if (index < MAX_OBJECTS) {
      objects[index] = *obj;// shallow copy is enough
      objects[index].index = index;
      printf("added object: %d\n", index);
      objectCount++;
    }
    else{
      printf("\nToo many objects!\n");
    }
    
}

// remove all whitespace in-place
void strip_whitespace(char *s) {
    char *d = s;
    while (*s) {
        if (!isspace(*s))
            *d++ = *s;
        s++;
    }
    *d = 0;
}

void load_object(const char *filename, struct object *obj) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror("file"); exit(1); }

    // ---- read full file into memory ----
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, f);
    buf[size] = 0;
    fclose(f);

    // ---- remove all whitespace ----
    strip_whitespace(buf);

    // ---- parse header ----
    char *p = buf;
    obj->vertex_count   = strtol(p, &p, 10);
    if (*p != ';') { printf("Syntax error\n"); return; }
    p++;

    obj->triangle_count = strtol(p, &p, 10);
    if (*p != ';') { printf("Syntax error\n"); return; }
    p++;

    // ---- allocate memory ----
    obj->points    = malloc(sizeof(double[3]) * obj->vertex_count);
    obj->triangles = malloc(sizeof(int[3])    * obj->triangle_count);
    obj->triColor  = malloc(sizeof(double[3]) * obj->triangle_count);

    // ---- parse points ----
    if (*p != '[') { printf("Expected '[' for points\n"); return; }
    p++;

    for (int i = 0; i < obj->vertex_count; i++) {
        if (*p != '{') { printf("Expected '{' for point\n"); return; }
        p++;

        obj->points[i][0] = strtod(p, &p);
        p++; // skip comma
        obj->points[i][1] = strtod(p, &p);
        p++;
        obj->points[i][2] = strtod(p, &p);

        if (*p != '}') { printf("Expected '}' in point\n"); return; }
        p++;

        if (*p == ',') p++; // skip comma between vectors
    }

    if (*p != ']') { printf("Expected ']' end of points\n"); return; }
    p++;

    // ---- parse triangle indices ----
    if (*p != '[') { printf("Expected '[' for triangles\n"); return; }
    p++;

    for (int i = 0; i < obj->triangle_count; i++) {
        if (*p != '{') { printf("Expected '{' for triangle\n"); return; }
        p++;

        obj->triangles[i][0] = strtol(p, &p, 10);
        p++;
        obj->triangles[i][1] = strtol(p, &p, 10);
        p++;
        obj->triangles[i][2] = strtol(p, &p, 10);

        if (*p != '}') { printf("Expected '}' in triangle\n"); return; }
        p++;

        if (*p == ',') p++;
    }

    if (*p != ']') { printf("Expected ']' end of triangles\n"); return; }
    p++;

    // ---- parse triangle colors ----
    if (*p != '[') { printf("Expected '[' for colors\n"); return; }
    p++;

    for (int i = 0; i < obj->triangle_count; i++) {
        if (*p != '{') { printf("Expected '{' for color\n"); return; }
        p++;

        obj->triColor[i][0] = strtod(p, &p);
        p++;
        obj->triColor[i][1] = strtod(p, &p);
        p++;
        obj->triColor[i][2] = strtod(p, &p);

        if (*p != '}') { printf("Expected '}' in color\n"); return; }
        p++;

        if (*p == ',') p++;
    }

    if (*p != ']') { printf("Expected ']' end of colors\n"); return; }

    free(buf);
}
