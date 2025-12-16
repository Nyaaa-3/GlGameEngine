#ifndef TRANSFORM_H
#define TRANSFORM_H

void transformXYZ(int vertex_count, float (*points)[3], float transformXYZ[3], int index);

void TransformRotation(int vertex_count, float (*points)[3], float ang[4]);

void Gravity(int vertex_count, float (*points)[3], float height, float deltatime, int index);

void CamGravity(float points[3], float height, float deltatime);

void CamJump(float jumpSpeed);

void xzCollition(int index, int status, float points[3], float camEstests[6]);
#endif
