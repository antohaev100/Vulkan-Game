#include "vk_fun.h"
#include "glfw_fun.h"

GLFWwindow *createVulkanWindow(const char *title){
	glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	int x,y,w,h;
	glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);
	int width = w-x, height = h-y;
	GLFWwindow *window = glfwCreateWindow(800, 600, title, NULL, VK_NULL_HANDLE);

	const GLFWvidmode *vidMode = glfwGetVideoMode(monitor);
	glfwSetWindowPos(window, (vidMode->width - width) / 2, (vidMode->height - height) / 2);

	return window;
}

void deleteWindow(GLFWwindow *pWindow){
	glfwDestroyWindow(pWindow);
}

static VkBool32 getSurfaceSupport(const VkSurfaceKHR surface, const VkPhysicalDevice physicalDevice, const uint32_t graphicsQueueFamilyindex){
	VkBool32 surfaceSupport = 0;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueueFamilyindex, surface, &surfaceSupport);
	return surfaceSupport;
}

VkSurfaceKHR createSurface(GLFWwindow *pWindow, const VkInstance instance, const VkPhysicalDevice physicalDevice, const uint32_t graphicsQueueFamilyindex){
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(instance, pWindow, VK_NULL_HANDLE, &surface);
	if(getSurfaceSupport(surface, physicalDevice, graphicsQueueFamilyindex) == VK_FALSE){
		printf("Surface does not support the graphics queue family\n");
		exit(EXIT_FAILURE);
	}
	return surface;
}

void deleteSurface(const VkInstance instance, VkSurfaceKHR *pSurface){
	vkDestroySurfaceKHR(instance, *pSurface, VK_NULL_HANDLE);
}

