#ifndef COLLITION_H
#define COLLITION_H

#include "../include/object.h"


extern float estests[MAX_OBJECTS][6];
extern float camEstests[6];

void camDefineBounds(float camPos[3]);
void getBoxes(struct object *things); 
int colliding(int index, int index2);
int camColliding(int index);
int camCollidingSpecial(int index);
int camCollidingBounce(int index);
int getGround(float points[3]);
void xzcolliding(float point[3]);
//void mainCollition(struct object *things);
#endif
