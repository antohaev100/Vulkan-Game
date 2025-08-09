#include "glfw_fun.h"
#include "utils.h"

static float pitch = 0.0f;
static double lastX, lastY;
static float sensitivity = 0.1f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    sharedBuffer *pBuffer = (sharedBuffer *)glfwGetWindowUserPointer(window);
    if(pBuffer == NULL){
        printf("Failed to get shared buffer\n");
        return;
    }

    static int firstMouse = 1;
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    pBuffer->yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    pBuffer->cameraFront[0] = sinf(radians(pBuffer->yaw)) * cosf(radians(pitch));
    pBuffer->cameraFront[1] = cosf(radians(pBuffer->yaw)) * cosf(radians(pitch));
    pBuffer->cameraFront[2] = sinf(radians(pitch));
    normalize(pBuffer->cameraFront);
}