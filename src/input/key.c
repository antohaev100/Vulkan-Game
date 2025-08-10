#include "glfw_fun.h"
#include "vulkan_game/utils.h"

static bool showCursor = false;
static bool fullscreen = false;
static int windowedPosX, windowedPosY, windowedWidth, windowedHeight;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Suppress unused parameter warnings
    (void)scancode;
    (void)mods;
    
    //Retrive the shared pBuffer
    sharedBuffer *pBuffer = (sharedBuffer *)glfwGetWindowUserPointer(window);
    if(pBuffer == NULL){
        printf("Failed to get shared pBuffer\n");
        return;
    }    
    if(action == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_ESCAPE:
                showCursor = !showCursor;
                if (showCursor) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                break;
            case GLFW_KEY_SPACE:
                pBuffer->cameraMoveInput[0] = true;
                break;
            case GLFW_KEY_LEFT_CONTROL:
                pBuffer->cameraMoveInput[1] = true;
                break;
            case GLFW_KEY_W:
                pBuffer->cameraMoveInput[2] = true;
                break;
            case GLFW_KEY_S:
                pBuffer->cameraMoveInput[3] = true;
                break;
           case GLFW_KEY_A:
                pBuffer->cameraMoveInput[4] = true;
                break;
            case GLFW_KEY_D:
                pBuffer->cameraMoveInput[5] = true;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                pBuffer->cameraMoveInput[6] = true;
                break;
            case GLFW_KEY_TAB:
                pBuffer->thirdPerson = !pBuffer->thirdPerson;
                break;
            case GLFW_KEY_1:
                pBuffer->debugInput[0] = true;
                break;
            case GLFW_KEY_2:
                pBuffer->debugInput[1] = true;
                break;
            case GLFW_KEY_3:
                pBuffer->debugInput[2] = true;
                break;
            case GLFW_KEY_F12:
                fullscreen = !fullscreen;
                if (!fullscreen) {
                    // Restore windowed mode
                    glfwSetWindowMonitor(window, NULL, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0);
                } else {
                    // Store windowed mode properties
                    glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
                    glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

                    // Get the primary monitor and its video mode
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

                    // Switch to fullscreen mode
                    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                }
                break;
        }
    }
    if(action == GLFW_RELEASE){
        switch(key){
            case GLFW_KEY_SPACE:
                pBuffer->cameraMoveInput[0] = false;
                break;
            case GLFW_KEY_LEFT_CONTROL:
                pBuffer->cameraMoveInput[1] = false;
                break;
            case GLFW_KEY_W:
                pBuffer->cameraMoveInput[2] = false;
                break;
            case GLFW_KEY_S:
                pBuffer->cameraMoveInput[3] = false;
                break;
            case GLFW_KEY_A:
                pBuffer->cameraMoveInput[4] = false;
                break;
            case GLFW_KEY_D:
                pBuffer->cameraMoveInput[5] = false;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                pBuffer->cameraMoveInput[6] = false;
                break;
            case GLFW_KEY_1:
                pBuffer->debugInput[0] = false;
                break;
            case GLFW_KEY_2:
                pBuffer->debugInput[1] = false;
                break;
            case GLFW_KEY_3:
                pBuffer->debugInput[2] = false;
                break;
        }
    }
}