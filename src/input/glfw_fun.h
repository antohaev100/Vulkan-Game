#ifndef GLFW_FUN_H
#define GLFW_FUN_H

#include "vulkan_game/std_c.h"
#include "vulkan_game/std_c.h"

GLFWwindow *createVulkanWindow(const char *title);
void deleteWindow(GLFWwindow *pWindow);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

#endif // GLFW_FUN_H
