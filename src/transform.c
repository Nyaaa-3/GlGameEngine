#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/object.h"
#include "../include/collitions.h"

float velocity_y[MAX_OBJECTS + 1];
double currentDY[MAX_OBJECTS + 1];
int onGround[MAX_OBJECTS + 1];
int jumping = 0;

void CamJump(float jumpSpeed){
    if (onGround[MAX_OBJECTS]) {
        velocity_y[MAX_OBJECTS] = -jumpSpeed;
        onGround[MAX_OBJECTS] = 0;
        jumping = 1;
    }

}


void xzCollition(int index, int status, float points[3], float camEstests[6]){
  float diff[4];
  int closest[4] = {0,0,0,0};
  
  diff[0] = (estests[index][0] - points[0]);
  diff[1] = (estests[index][2] - points[2]);
  diff[2] = (estests[index][3] - points[0]);
  diff[3] = (estests[index][5] - points[2]);
  
  for(int i = 0; i < 4; i++){
    if(diff[i] < 0){
      diff[i] *= -1;
    }
    printf("%f ", diff[i]);
  }

  for(int i = 0; i < 4; i++){
    if(diff[i] < 0.2){
      closest[i] = 1;
    }
  }

  printf("%d ", closest[0]);
  printf("%d ", closest[1]);
  printf("%d ", closest[2]);
  printf("%d \n", closest[3]);
  //printf("  %d\n",closest);
  if(closest[0] == 1 && (points[0] > estests[index][0])){
    points[0] = estests[index][0] + 0.2;
  }
  if(closest[1] == 1 && (points[2] > estests[index][2])){
    points[2] = estests[index][2] + 0.2;
  }
  if(closest[2] == 1 && (points[0] < estests[index][3])){
    points[0] = estests[index][3] - 0.2;
  }
  if(closest[3] == 1 && (points[2] < estests[index][5])){
    points[2] = estests[index][5] - 0.2;
  }
}

int lastGround; 
void CamGravity(float points[3], float height, float deltatime){
  float acceleration_y = 9.8;
  int ground = getGround(points);
  int state = camColliding(ground);


  if(ground != lastGround){
    onGround[MAX_OBJECTS] = 0;
  }
  lastGround = ground;
 
  if(!onGround[MAX_OBJECTS]){
    velocity_y[MAX_OBJECTS] += acceleration_y * deltatime;
    currentDY[MAX_OBJECTS] = velocity_y[MAX_OBJECTS] * deltatime;
    
    points[1] -= currentDY[MAX_OBJECTS];
  }

    state = camColliding(ground);
    
    if((state != 2) && velocity_y[MAX_OBJECTS] > 0){
      points[1] = estests[ground][1] + 0.5;

      onGround[MAX_OBJECTS] = 1;
      jumping = 0;
      velocity_y[MAX_OBJECTS] = 0;
      currentDY[MAX_OBJECTS] = 0;
    }
}

void Gravity(int vertex_count, float (*points)[3], float height, float deltatime, int index){
  float lowest = estests[index][4];
  float acceleration_y = 9.8;

  
  if(onGround[index] == 0){
    velocity_y[index] += acceleration_y * deltatime;
    currentDY[index] = velocity_y[index] * deltatime;
    
    if(estests[index][4] > height){

      for(int i = 0; i < vertex_count; i++){
        points[i][1] -= currentDY[index];
      }
      estests[index][1] -= currentDY[index];
      estests[index][4] -= currentDY[index];
    }
    else if(estests[index][4] < height){
      double tans = height - estests[index][4];

      for(int i = 0; i < vertex_count; i++){
        points[i][1] += tans;
      }
      estests[index][1] += tans;
      estests[index][4] += tans;
    }
    else{
      onGround[index] = 1;
      velocity_y[index] = 0;
      currentDY[index] = 0;
    }
  }
  
}

void transformXYZ(int vertex_count, float (*points)[3], float transformXYZ[3], int index){
    for(int i = 0; i < vertex_count; i++){
        for(int j = 0; j < 3; j++){
            points[i][j] += transformXYZ[j];
        }
    }
    for(int j = 0; j < 3; j++){
      estests[index][j] += transformXYZ[j];
      estests[index][j + 3] += transformXYZ[j];
    }
}

void TransformRotation(int vertex_count, float (*points)[3], float ang[4]){

  double rotQuat[4] = {cos(ang[0]), sin(ang[0]) * ang[1], sin(ang[0]) * ang[2], sin(ang[0]) * ang[3]};
  double conjugate[4]; 
  conjugate[0] = rotQuat[0];
  conjugate[1] = -rotQuat[1]; 
  conjugate[2] = -rotQuat[2]; 
  conjugate[3] = -rotQuat[3]; 
  
  double quat[4];
  
  for(int i = 0; i < vertex_count; i++){
    quat[0] = -(conjugate[1] * points[i][0]) - (conjugate[2] * points[i][1]) - (conjugate[3] * points[i][2]);
    quat[1] =  (conjugate[0] * points[i][0]) - (conjugate[2] * points[i][2]) + (conjugate[3] * points[i][1]);
    quat[2] =  (conjugate[0] * points[i][1]) + (conjugate[1] * points[i][2]) - (conjugate[3] * points[i][0]);
    quat[3] =  (conjugate[0] * points[i][2]) - (conjugate[1] * points[i][1]) + (conjugate[2] * points[i][0]);

    points[i][0] = (quat[0] * rotQuat[1]) + (quat[1] * rotQuat[0]) - (quat[2] * rotQuat[3]) + (quat[3] * rotQuat[2]);
    points[i][1] = (quat[0] * rotQuat[2]) + (quat[1] * rotQuat[3]) + (quat[2] * rotQuat[0]) - (quat[3] * rotQuat[1]);
    points[i][2] = (quat[0] * rotQuat[3]) - (quat[1] * rotQuat[2]) + (quat[2] * rotQuat[1]) + (quat[3] * rotQuat[0]);
  }
}

