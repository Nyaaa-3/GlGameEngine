#include <stdio.h>
#include "../include/object.h"
#include "../include/camera.h"
#include "../include/engine.h"
#include "../include/collitions.h"
#include "../include/transform.h"

float estests[MAX_OBJECTS][6];
float camEstests[6];

int getGround(float points[3]){
  int possible[MAX_OBJECTS];
  int counter = 0;
  int ground;
  int collide;

  for(int i = 0; i < objectCount; i++){
    collide = camCollidingSpecial(i);
    if(collide == 2 || collide == 4 || collide == 1){
      possible[counter] = i;
      counter++;
    }
  }

  if(counter == 0){
    return 0;  
  }
  ground = possible[0];

  for(int i = 0; i < counter; i++){
    if(estests[possible[i]][1] > estests[ground][1]){
      ground = possible[i];
    }
  }
  return ground;
}

void camDefineBounds(float camPos[3]){
  camEstests[0] = camPos[0] + 0.2;
  camEstests[1] = camPos[1] + 0.2;
  camEstests[2] = camPos[2] + 0.2;
  camEstests[3] = camPos[0] - 0.2;
  camEstests[4] = camPos[1] - 0.5;
  camEstests[5] = camPos[2] - 0.2;
}

void defineBounds(int index, struct object obj){
  estests[index][0] = obj.points[0][0]; 
  estests[index][1] = obj.points[0][1];
  estests[index][2] = obj.points[0][2];
  estests[index][3] = obj.points[0][0];
  estests[index][4] = obj.points[0][1];
  estests[index][5] = obj.points[0][2];

  for(int i = 0; i < obj.vertex_count; i++){
    if(obj.points[i][0] > estests[index][0]){
      estests[index][0] = obj.points[i][0];
    }
    if(obj.points[i][1] > estests[index][1]){
      estests[index][1] = obj.points[i][1];
    }
    if(obj.points[i][2] > estests[index][2]){
      estests[index][2] = obj.points[i][2];
    }
    if(obj.points[i][0] < estests[index][3]){
      estests[index][3] = obj.points[i][0];
    }
    if(obj.points[i][1] < estests[index][4]){
      estests[index][4] = obj.points[i][1];
    }
    if(obj.points[i][2] < estests[index][5]){
      estests[index][5] = obj.points[i][2];
    }
  }
}

int camCollidingBounce(int index){
    float *A = estests[index];
    float *B = camEstests;
  
    // AABB overlap test
    int overlapX = (A[3] <= B[0]) && (A[0] >= B[3]);
    int overlapY = (A[4] <= B[1]) && (A[1] >= B[4]);
    int overlapZ = (A[5] <= B[2]) && (A[2] >= B[5]);
    
    //inside
    if((overlapX && overlapZ) && (A[1] - B[4] > 0.1) && (B[1] > A[4])){
      return 1;
    }
    else{
      return 0;
    }
}

void xzcolliding(float points[3]){

  for(int i = 0; i < objectCount; i++){
    if(camCollidingBounce(i) == 1){
      xzCollition(i, 1, points, camEstests);
    }
  }
}

int camCollidingSpecial(int index){
    float *A = estests[index];
    float *B = camEstests;
  
    // AABB overlap test
    int overlapX = (A[3] <= B[0]) && (A[0] >= B[3]);
    int overlapY = (A[4] <= B[1]) && (A[1] >= B[4]);
    int overlapZ = (A[5] <= B[2]) && (A[2] >= B[5]);
    
    //inside
    if((overlapX && overlapZ) && ((A[1] - B[4]) < 0.1)){
      return 4;
    }
    if((overlapX && overlapZ) && (A[1] == B[4])){
      return 1;
    }
    //over
    else if((overlapX && overlapZ) && (B[4] > A[1])){
      return 2;
    }
    //under
    else if((overlapX && overlapZ) && (B[1] < A[4])){
      return 3;
    }
    else{
      return 0;
    }
}

int camColliding(int index){
    float *A = estests[index];
    float *B = camEstests;
  
    // AABB overlap test
    int overlapX = (A[3] <= B[0]) && (A[0] >= B[3]);
    int overlapY = (A[4] <= B[1]) && (A[1] >= B[4]);
    int overlapZ = (A[5] <= B[2]) && (A[2] >= B[5]);
    
    //inside


    if((overlapX && overlapY && overlapZ) == 1){
      return 1;
    }
    //over
    else if((overlapX && overlapZ) && (B[4] > A[1])){
      return 2;
    }
    //under
    else if((overlapX && overlapZ) && (B[4] < A[1])){
      return 3;
    }
    else{
      return 0;
    }
}

int colliding(int index, int index2){
    float *A = estests[index];
    float *B = estests[index2];

    // AABB overlap test
    int overlapX = (A[3] <= B[0]) && (A[0] >= B[3]);
    int overlapY = (A[4] <= B[1]) && (A[1] >= B[4]);
    int overlapZ = (A[5] <= B[2]) && (A[2] >= B[5]);

    return overlapX && overlapY && overlapZ;
}

void getBoxes(struct object *things){

  for(int i = 0; i < objectCount; i++){  
    defineBounds(i, things[i]);
    //printf("object %d: ", i);
    //for(int j = 0; j < 6; j++){
      //printf("%f, ", estests[i][j]);
    //}
    //printf("\n");
  }

}

