#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 158
#define SCREEN_HEIGHT 44
#define MAX_DEPTH 0 
#define CUBE_WIDTH 15

int k1 = 40;

float Alpha, Beta, Theta;   //angles of orientation of the object
//float A,B,C;                //angles of orientation of the camera
//float surface[3];           //position of the projection surface relative to the camera

char surface2D[SCREEN_WIDTH * SCREEN_HEIGHT];    //projection surface
float zBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

char backgroundASCIICode = ' ';
int distanceFromCamera = 100;
int horizontalOffset = - CUBE_WIDTH;
float speed = 0.5;

// ================= Cube Rotation Ricalc Position

float calculateX(int i, int j, int k) {
  return j * sin(Alpha) * sin(Beta) * cos(Theta) - k * cos(Alpha) * sin(Beta) * cos(Theta) +
         j * cos(Alpha) * sin(Theta) + k * sin(Alpha) * sin(Theta) + i * cos(Beta) * cos(Theta);
}

float calculateY(int i, int j, int k) {
  return j * cos(Alpha) * cos(Theta) + k * sin(Alpha) * cos(Theta) -
         j * sin(Alpha) * sin(Beta) * sin(Theta) + k * cos(Alpha) * sin(Beta) * sin(Theta) -
         i * cos(Beta) * sin(Theta);
}

float calculateZ(int i, int j, int k) {
  return k * cos(Alpha) * cos(Beta) - j * sin(Alpha) * cos(Beta) + i * sin(Beta);
}

// ================== Cube Rotation Ricalc Position
/*
// ================== Object Project Coordinates Ricalc

float calculateDx(int object[], int camera[]){
    return cos(B) * cos(C) * (object[0] - camera[0]) + cos(B) * sin(C) * (object[1] - camera[1]) - sin(B) * (object[2] - camera[2]);
}

float calculateDy(int object[], int camera[]){
    return (sin(A) * sin(B) * cos(C) - cos(A) * sin(C)) * (object[0] - camera[0]) + (sin(A) * sin(B) * sin(C) + cos(A) * cos(C)) * (object[1] * camera[1]) + sin(A) * cos(B) * (object[2] * camera[2]);
}

float calculateDz(int object[], int camera[]){
    return (cos(A) * sin(B) * cos(C) + sin(A) * sin(C)) * (object[0] - camera[0]) + (cos(A) * sin(B) * sin(C) - sin(A) * cos(C)) * (object[1] - camera[1]) + cos(A) * cos(B) * (object[2] * camera[2]);
}

float calculateXprojection(float f[]){
    if(f[2] < 1 && f[2] > -1){
        return 0;
    }else{
        return f[0] / f[2];
    }
}

float calculateYprojection(float f[]){    
    if(f[2] < 1 && f[2] > -1){
        return 0;
    }else{
        return f[1] / f[2];
    }
}

void homogeneousCoordinates(int object[], int camera[], float f[]){
    float d[3];
    
    // calculate the position of the object relative to the camera
    d[0] = calculateDx(object, camera);
    d[1] = calculateDy(object, camera);
    d[2] = calculateDz(object, camera);

    f[0] = (d[0] * surface[2] + d[2] * surface[0]) / (surface[2]);
    f[1] = (d[1] * surface[2] + d[2] * surface[1]) / (surface[2]);
    f[2] = d[2] / surface[2];
}

void calculateUpdatedProjectionCoordinates(int object[], int camera[], int projection[]){
    float f[3];
    homogeneousCoordinates(object, camera, f);
    
    projection[0] = calculateXprojection(f);
    projection[1] = calculateYprojection(f);
}

// ================== Object Project Coordinates Ricalc
*/
// ================== Main calc

void calculateProjection(float x, float y, float z, char symbol){
    float rotatedX = calculateX(x, y, z);
    float rotatedY = calculateY(x, y, z);
    float rotatedZ = calculateZ(x, y, z) + distanceFromCamera;
    
  //  calculateUpdatedProjectionCoordinates((int[]){rotatedX, rotatedY, rotatedZ}, (int[]){0, 0, 0}, projection);
     
    if(rotatedZ != 0){
        int xp = (int)  ((rotatedX/rotatedZ) * k1 * 2 + horizontalOffset + SCREEN_WIDTH/2);
        int yp = (int) ((rotatedY/rotatedZ) * k1  + SCREEN_HEIGHT/2);

        int idx = xp + yp * SCREEN_WIDTH;

        if(idx >= 0 && idx < SCREEN_WIDTH * SCREEN_HEIGHT){
            if(1/rotatedZ > zBuffer[idx]){
                zBuffer[idx] = 1 / rotatedZ;
                surface2D[idx] = symbol;
            }
        }
    }
    
}

/*
void initializer(float camera[]){
    surface[0] = 0;
    surface[1] = 0;
    surface[2] = 1; // Set to 1 to avoid division by zero

    Alpha = A = 0;
    Beta = B = 0;
    Theta = C = 0;

    camera[0] = SCREEN_WIDTH / 2;
    camera[1] = SCREEN_HEIGHT / 2;
    camera[2] = -50; // Set a negative value to place the camera behind the cube
}
*/

int main(){
    printf("\e[1;1H\e[2J");

    //float camera[3];

    //initializer(camera);
    while(1){
        memset(surface2D, backgroundASCIICode, SCREEN_WIDTH * SCREEN_HEIGHT);
        memset(zBuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);

        for(float cubeX = -CUBE_WIDTH;  cubeX < CUBE_WIDTH; cubeX += speed){
            for(float cubeY = -CUBE_WIDTH;  cubeY < CUBE_WIDTH; cubeY += speed){

                calculateProjection(cubeX, cubeY, -CUBE_WIDTH, '#');
                calculateProjection(CUBE_WIDTH, cubeY, cubeX, '$');
                calculateProjection(-CUBE_WIDTH, cubeY, -cubeX, '~');
                calculateProjection(-cubeX, cubeY, CUBE_WIDTH, '@');
                calculateProjection(cubeX, -CUBE_WIDTH, -cubeY, ';');
                calculateProjection(cubeX, CUBE_WIDTH, cubeY, '+');
            }
        }
        
        printf("\x1b[H");

        for (int k = 0; k < SCREEN_WIDTH * SCREEN_HEIGHT; k++) {
            putchar(k % SCREEN_WIDTH ? surface2D[k] : '\n');
        }

        Alpha += 0.05;
        Beta += 0.05;
        Theta += 0.01;

        usleep(3000); // Add a small delay to control the speed of the rotation
    }

    return 0;
}
