#include "vulkan_game/shared_buffer.h"
#include "vulkan_game/utils.h"

static const float Up[] = {0.0f, 0.0f, 1.0f}; // Up direction for the game
static float nextPlayerPos[3] = {0.0f, 0.0f, 0.25f};
static float nextRotation[3] = {0.0f, 0.0f, 0.0f};
static float gravitySpeed = 0.0f;
static float playerSpeed = 1.0f;

//debug
static int frameindex = 0;


static void movePlayer(sharedBuffer *pBuffer){
        pBuffer->playerPos[0] = lerp(pBuffer->playerPos[0], nextPlayerPos[0], 0.001f, pBuffer->dt, 0.001f);
        pBuffer->playerPos[1] = lerp(pBuffer->playerPos[1], nextPlayerPos[1], 0.001f, pBuffer->dt, 0.001f);
        pBuffer->playerPos[2] = lerp(pBuffer->playerPos[2], nextPlayerPos[2], 0.001f, pBuffer->dt, 0.001f);
        pBuffer->playerModel.rotation[0] = lerpDegrees(pBuffer->playerModel.rotation[0], nextRotation[0], 0.001f, pBuffer->dt, 0.4f);
        pBuffer->playerModel.rotation[1] = lerpDegrees(pBuffer->playerModel.rotation[1], nextRotation[1], 0.001f, pBuffer->dt, 0.4f);
        pBuffer->playerModel.rotation[2] = lerpDegrees(pBuffer->playerModel.rotation[2], nextRotation[2], 0.001f, pBuffer->dt, 0.4f);

        memcpy(pBuffer->playerModel.pos, pBuffer->playerPos, sizeof(pBuffer->playerPos));
}

static void playerPosMapCollision(sharedBuffer *pBuffer){
    if(pBuffer->playerPos[0] < -9.75f){
        pBuffer->playerState = ON_WALL_X_NEG;
        pBuffer->playerModel.rotation[1] = atan2f(pBuffer->playerPos[1] - nextPlayerPos[1], pBuffer->playerPos[2] - nextPlayerPos[2]);

        nextPlayerPos[0] = -9.75f;
        pBuffer->playerPos[0] = -9.75f;
    } else if(pBuffer->playerPos[0] > 9.75f){
        pBuffer->playerState = ON_WALL_X_POS;
        pBuffer->playerModel.rotation[1] = atan2f(nextPlayerPos[1] - pBuffer->playerPos[1], nextPlayerPos[2] - pBuffer->playerPos[2]);
        nextPlayerPos[0] = 9.75f;
        pBuffer->playerPos[0] = 9.75f;
    }

    if(pBuffer->playerPos[1] < -9.75f){
        pBuffer->playerState = ON_WALL_Y_NEG;
        pBuffer->playerModel.rotation[1] = atan2f(pBuffer->playerPos[0] -nextPlayerPos[0], pBuffer->playerPos[2] - nextPlayerPos[2]);
        nextPlayerPos[1] = -9.75f;
        pBuffer->playerPos[1] = -9.75f;
    } else if(pBuffer->playerPos[1] > 9.75f){
        pBuffer->playerState = ON_WALL_Y_POS;
        pBuffer->playerModel.rotation[1] = atan2f(nextPlayerPos[0] - pBuffer->playerPos[0], nextPlayerPos[2]- pBuffer->playerPos[2]);
        nextPlayerPos[1] = 9.75f;
        pBuffer->playerPos[1] = 9.75f;
    }

    if(pBuffer->playerPos[2] < 0.25f){
        pBuffer->playerState = ON_GROUND;
        pBuffer->playerModel.rotation[2] = atan2f(pBuffer->playerPos[0] - nextPlayerPos[0], nextPlayerPos[1] - pBuffer->playerPos[1]);
        nextPlayerPos[2] = 0.25f;
        pBuffer->playerPos[2] = 0.25f;
    } else if (pBuffer->playerPos[2] > 9.75f){
        pBuffer->playerState = ON_CEILING;
        pBuffer->playerModel.rotation[2] = atan2f(pBuffer->playerPos[0] - nextPlayerPos[0], nextPlayerPos[1] - pBuffer->playerPos[1]);
        nextPlayerPos[2] = 9.75f;
        pBuffer->playerPos[2] = 9.75f;
    }
}

static void cameraPosMapCollision(float cameraPos[3]){
    if(cameraPos[0] < -9.75f){
        cameraPos[0] = -9.75f;
    } else if(cameraPos[0] > 9.75f){
        cameraPos[0] = 9.75f;
    }

    if(cameraPos[1] < -9.75f){
        cameraPos[1] = -9.75f;
    } else if(cameraPos[1] > 9.75f){
        cameraPos[1] = 9.75f;
    }

    if(cameraPos[2] < 0.25f){
        cameraPos[2] = 0.25f;
    } else if (cameraPos[2] > 9.75f){
        cameraPos[2] = 9.75f;
    }
}

static void updateCamera(float cameraPos[3], const float cameraFront[3], float cameraTarget[3], const float playerPos[3], const bool thirdPerson){
    memcpy(cameraPos, playerPos, sizeof(float) * 3);
    if(thirdPerson){
        float distance = 1.5f;
        cameraPos[0] -= distance * cameraFront[0];
        cameraPos[1] -= distance * cameraFront[1];
        cameraPos[2] -= distance * cameraFront[2];
        float cameraRight[3];
        crossProduct(cameraFront, Up, cameraRight);
        normalize(cameraRight);
        // Offset the camera position to the left
        float offset = 0.3f; // Adjust this value as needed
        cameraPos[0] += offset * cameraRight[0];
        cameraPos[1] += offset * cameraRight[1];
        cameraPos[2] += offset * cameraRight[2];

        // Calculate cameraUp vector
        float cameraUp[3];
        crossProduct(cameraRight, cameraFront, cameraUp);
        normalize(cameraUp);

        // Offset the camera position downwards
        float verticalOffset = 0.3f; // Adjust this value as needed
        cameraPos[0] += verticalOffset * cameraUp[0];
        cameraPos[1] += verticalOffset * cameraUp[1];
        cameraPos[2] += verticalOffset * cameraUp[2];
        cameraPosMapCollision(cameraPos);
    }

    cameraTarget[0] = cameraPos[0] + cameraFront[0];
    cameraTarget[1] = cameraPos[1] + cameraFront[1];
    cameraTarget[2] = cameraPos[2] + cameraFront[2];
}

static void updateSpeed(float diff, const bool moving, const float dt){
    if(diff > PI) diff -= 2*PI;
    if(diff < -PI) diff += 2*PI;
    if(moving && diff <  0.6f*PI){
        playerSpeed += dt;
        if(playerSpeed > 20.0f) playerSpeed = 20.0f;
    } else {
        playerSpeed -= playerSpeed * 20 * dt;
        if(playerSpeed < 1.0f) playerSpeed = 1.0f;
    }
}

static bool isMoving(const bool i[7]){
    return (!i[2] != !i[3]) || (!i[4] != !i[5]);
}

static void onGroundInput(const bool i[7], const float speeds[2], const float rotationspeed, possiblePlayerStates *playerState, const float playerPos[3], const float playerRotation[3], const float dt){
    if(i[0]){
        nextPlayerPos[2] += 2.0f;
        *playerState = IN_AIR;
    }

    nextPlayerPos[0] += speeds[0] * dt;
    nextPlayerPos[1] += speeds[1] * dt;
    bool moving = isMoving(i);
    nextRotation[0] -= rotationspeed *dt * moving;
    nextRotation[1] = 0.0f;
    nextRotation[2] = atan2f(playerPos[0] - nextPlayerPos[0], nextPlayerPos[1] - playerPos[1]);

    float diff = fmodf(nextRotation[2] - playerRotation[2], 2*PI);
    updateSpeed(diff, moving, dt);
}

static void onCeilingInput(const bool i[7], const float speeds[2], const float rotationspeed, possiblePlayerStates *playerState, const float playerPos[3], const float playerRotation[3], const float dt){
    if(i[0]){
        nextPlayerPos[2] -= 2.0f;
        *playerState = IN_AIR;
    }

    nextPlayerPos[0] += speeds[0] * dt;
    nextPlayerPos[1] += speeds[1] * dt;
    bool moving = isMoving(i);
    nextRotation[0] -= rotationspeed *dt * moving;
    nextRotation[1] = 0.0f;
    nextRotation[2] = atan2f(playerPos[0] - nextPlayerPos[0], nextPlayerPos[1] - playerPos[1]);

    float diff = fmodf(nextRotation[2] - playerRotation[2], 2*PI);
    updateSpeed(diff, moving, dt);
}

static void inAirInput(const float playerPos[3], const bool i[7], const float yaw, const float dt){
    float atan = atan2f(playerPos[0] - nextPlayerPos[0], nextPlayerPos[1] - playerPos[1]);
    float x = (i[2] - i[3]), y = (i[4] - i[5]);
    float length = sqrtf(x * x + y * y);
    if (length != 0.0f) {
        x /= length;
        y /= length;        
        float diff = fmodf(radians(yaw) - atan2f(y,x) + atan, 2*PI);
        if(diff > PI) diff -= 2*PI;
        if(diff < -PI) diff += 2*PI;
        float sidewaySpeed = playerSpeed * 3 * PI * dt;
        sidewaySpeed*= diff > 0 ? 1 : -1;
        if(fabs(diff) > fabs(sidewaySpeed)){
            atan-= sidewaySpeed;
            nextRotation[2] -= sidewaySpeed/2;
        } else {
            atan-= diff;
            nextRotation[2] -= diff/2;
        }
    }
    float down = playerSpeed * i[1] * dt;
    nextPlayerPos[0] += playerSpeed * sinf(-atan) * dt;
    nextPlayerPos[1] += playerSpeed * cosf(atan) * dt;
    nextPlayerPos[2] += gravitySpeed * dt - down;
    gravitySpeed -= 15.0f * dt;
    nextRotation[0] -= down * 10;
    nextRotation[1] = 0.0f;
}

static void onWallYPosInput(const bool i[7], const float speeds[2], const float rotationspeed, possiblePlayerStates *playerState, const float playerPos[3], const float playerRotation[3], const float dt){
    if(i[0]){
        nextPlayerPos[1] -= 2.0f;
        *playerState = IN_AIR;
    }
    nextPlayerPos[0] += speeds[0] * dt;
    nextPlayerPos[2] += speeds[1] * dt;
    bool moving = isMoving(i);
    nextRotation[0] -= rotationspeed * dt * moving;
    nextRotation[1] = atan2f(nextPlayerPos[0] - playerPos[0], nextPlayerPos[2]- playerPos[2]);
    nextRotation[2] = 0.0f;

    float diff = fmodf(nextRotation[1] - playerRotation[1], 2*PI);
    updateSpeed(diff, moving, dt);
}

static void onWallYNegInput(const bool i[7], const float speeds[2], const float rotationspeed, possiblePlayerStates *playerState, const float playerPos[3], const float playerRotation[3], const float dt){
    if(i[0]){
        nextPlayerPos[1] += 2.0f;
        *playerState = IN_AIR;
    }
    nextPlayerPos[0] += speeds[0] * dt;
    nextPlayerPos[2] -= speeds[1] * dt;
    bool moving = isMoving(i);
    nextRotation[0] -= rotationspeed * dt * moving;
    nextRotation[1] = atan2f(playerPos[0] -nextPlayerPos[0], playerPos[2] - nextPlayerPos[2]);
    nextRotation[2] = 0.0f;

    float diff = fmodf(nextRotation[1] - playerRotation[1], 2*PI);
    updateSpeed(diff, moving, dt);
}

static void onWallXPosInput(const bool i[7], const float speeds[2], const float rotationspeed, possiblePlayerStates *playerState, const float playerPos[3], const float playerRotation[3], const float dt){
    if(i[0]){
        nextPlayerPos[0] -= 2.0f;
        *playerState = IN_AIR;
    }
    nextPlayerPos[1] += speeds[1] * dt;
    nextPlayerPos[2] += speeds[0] * dt;
    bool moving = isMoving(i);
    nextRotation[0] += rotationspeed * dt * moving;
    nextRotation[1] = atan2f(nextPlayerPos[1] - playerPos[1], nextPlayerPos[2] - playerPos[2]);
    nextRotation[2] = PI/2;

    float diff = fmodf(nextRotation[1] - playerRotation[1], 2*PI);
    updateSpeed(diff, moving, dt);
}

static void onWallXNegInput(const bool i[7], const float speeds[2], const float rotationspeed, possiblePlayerStates *playerState, const float playerPos[3], const float playerRotation[3], const float dt){
    if(i[0]){
        nextPlayerPos[0] += 2.0f;
        *playerState = IN_AIR;
    }
    nextPlayerPos[1] += speeds[1] * dt;
    nextPlayerPos[2] -= speeds[0] * dt;
    bool moving = isMoving(i);
    nextRotation[0] += rotationspeed * dt * moving;
    nextRotation[1] = atan2f(playerPos[1] - nextPlayerPos[1], playerPos[2] - nextPlayerPos[2]);
    nextRotation[2] = PI/2;

    float diff = fmodf(nextRotation[1] - playerRotation[1], 2*PI);
    updateSpeed(diff, moving, dt);
}

static void input(sharedBuffer *pBuffer){

    pBuffer->fov = lerp(pBuffer->fov, playerSpeed * 2.5f + 70.0f, 0.0001f, pBuffer->dt, 0.5f);
    if(pBuffer->playerState != IN_AIR){
        gravitySpeed = 0;
    }
    const float s = pBuffer->cameraFront[0];
    const float c = pBuffer->cameraFront[1];
    const bool *i = pBuffer->cameraMoveInput;
    float speeds[2] = {playerSpeed * (s*(i[2]-i[3]) + c*(i[5]-i[4])), playerSpeed * (s*(i[4]-i[5]) + c*(i[2]-i[3]))};
    float rotationspeed = playerSpeed * 2 / pBuffer->playerModel.dimension[0];


    switch (pBuffer->playerState) {
        case ON_GROUND:
            onGroundInput(pBuffer->cameraMoveInput, speeds, rotationspeed, &pBuffer->playerState, pBuffer->playerPos, pBuffer->playerModel.rotation, pBuffer->dt);
            break;
        case IN_AIR:
            inAirInput(pBuffer->playerPos, pBuffer->cameraMoveInput, pBuffer->yaw, pBuffer->dt);
            break;
        case ON_WALL_Y_POS:
            onWallYPosInput(pBuffer->cameraMoveInput, speeds, rotationspeed, &pBuffer->playerState, pBuffer->playerPos, pBuffer->playerModel.rotation, pBuffer->dt);
            break;
        case ON_WALL_Y_NEG:
            onWallYNegInput(pBuffer->cameraMoveInput, speeds, rotationspeed, &pBuffer->playerState, pBuffer->playerPos, pBuffer->playerModel.rotation, pBuffer->dt);
            break;
        case ON_WALL_X_POS:
            onWallXPosInput(pBuffer->cameraMoveInput, speeds, rotationspeed, &pBuffer->playerState, pBuffer->playerPos, pBuffer->playerModel.rotation, pBuffer->dt);
            break;
        case ON_WALL_X_NEG:
            onWallXNegInput(pBuffer->cameraMoveInput, speeds, rotationspeed, &pBuffer->playerState, pBuffer->playerPos, pBuffer->playerModel.rotation, pBuffer->dt);
            break;
        case ON_CEILING:
            onCeilingInput(pBuffer->cameraMoveInput, speeds, rotationspeed, &pBuffer->playerState, pBuffer->playerPos, pBuffer->playerModel.rotation, pBuffer->dt);
            break;
    }
}

void update(sharedBuffer *pBuffer){
    input(pBuffer);
    playerPosMapCollision(pBuffer);
    updateCamera(pBuffer->cameraPos, pBuffer->cameraFront, pBuffer->cameraTarget, pBuffer->playerPos, pBuffer->thirdPerson);
    movePlayer(pBuffer);
    frameindex++;
}