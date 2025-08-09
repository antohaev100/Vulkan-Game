#ifndef SHARED_BUFFER_H
#define SHARED_BUFFER_H
#include "std_c.h"

typedef enum PossiblePlayerStates {
    ON_GROUND,
    IN_AIR,
    ON_WALL_Y_POS,
    ON_WALL_Y_NEG,
    ON_WALL_X_POS,
    ON_WALL_X_NEG,
    ON_CEILING
} possiblePlayerStates;

typedef struct DynamicArray {
    void* array;    // array of elements
    int elemSize;   // size of each element
    int n;          // number of elements
    int c;          // capacity
    int minc;       // minimum capacity
} vec;

typedef struct ThreeDimensionalObject {
    float pos[3]; //xyz (center)
    float dimension[3]; //xyz (depth,width,height)
    float color[3]; //rgb (red,green,blue)
    float rotation[3]; //xyz (pitch,yaw,roll)
} obj3d;

typedef struct SharedBuffer {
    float dt;
    obj3d playerModel;
    vec cuboids;
    vec ellipsoids;
    vec ellipsoidCylinders;
    float yaw; //xy (degrees)
    float cameraPos[3]; //xyz
    float playerPos[3]; //xyz
    float cameraFront[3];   //xyz
    float cameraTarget[3];  //xyz
    bool cameraMoveInput[7]; //space,left-ctrl,w,s,a,d,shift
    bool debugInput[3]; //1,2,3
    possiblePlayerStates playerState;
    float fov; //degrees
    bool thirdPerson;
} sharedBuffer;

void update(sharedBuffer *pBuffer);
#endif // SHARED_BUFFER_H