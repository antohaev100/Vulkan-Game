#include "vk_fun.h"

VkInstance createInstance(){
	VkApplicationInfo applicationInfo = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		VK_NULL_HANDLE,
		VK_NULL_HANDLE,
		0,
		VK_NULL_HANDLE,
		0,
		VK_API_VERSION_1_3
	};

	const char layerList[][VK_MAX_EXTENSION_NAME_SIZE] = {
		"VK_LAYER_KHRONOS_validation"
	};
	const char *layers[] = {
		layerList[0]
	};

    uint32_t layerCount = sizeof(layers) / sizeof(layers[0]);

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Additional extensions you want to enable
    const char *additionalExtensions[] = {
		VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
		VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME,
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    };
    uint32_t additionalExtensionCount = sizeof(additionalExtensions) / sizeof(additionalExtensions[0]);

    // Combine GLFW extensions and additional extensions
    uint32_t totalExtensionCount = glfwExtensionCount + additionalExtensionCount;
    const char **extensions = (const char **)malloc(totalExtensionCount * sizeof(const char *));
    for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
        extensions[i] = glfwExtensions[i];
    }
    for (uint32_t i = 0; i < additionalExtensionCount; ++i) {
        extensions[glfwExtensionCount + i] = additionalExtensions[i];
    }

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = layerCount,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = totalExtensionCount,
        .ppEnabledExtensionNames = extensions
    };

	VkInstance instance;
	if(vkCreateInstance(&instanceCreateInfo, VK_NULL_HANDLE, &instance) != VK_SUCCESS){
        printf("failed to create instance\n");
        exit(EXIT_FAILURE);
    }
	return instance;
}

void deleteInstance(VkInstance *pInstance){
	vkDestroyInstance(*pInstance, VK_NULL_HANDLE);
}
