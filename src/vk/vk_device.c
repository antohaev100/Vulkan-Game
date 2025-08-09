#include "vk_fun.h"

VkDevice createDevice(const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyNumber, const VkQueueFamilyProperties *queueFamilyProperties){
	VkDeviceQueueCreateInfo *deviceQueueCreateInfo = (VkDeviceQueueCreateInfo *)malloc(queueFamilyNumber * sizeof(VkDeviceQueueCreateInfo));
	float **queuePriorities = (float **)malloc(queueFamilyNumber * sizeof(float *));

	for(uint32_t i = 0; i < queueFamilyNumber; i++){
		queuePriorities[i] = (float *)malloc(queueFamilyProperties[i].queueCount * sizeof(float));
		for(uint32_t j = 0; j < queueFamilyProperties[i].queueCount; j++){
			queuePriorities[i][j] = 1.0f;
		}

		deviceQueueCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo[i].pNext = VK_NULL_HANDLE;
		deviceQueueCreateInfo[i].flags = 0;
		deviceQueueCreateInfo[i].queueFamilyIndex = i;
		deviceQueueCreateInfo[i].queueCount = queueFamilyProperties[i].queueCount;
		deviceQueueCreateInfo[i].pQueuePriorities = queuePriorities[i];
	}

    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME
    };

	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	
	VkDeviceCreateInfo deviceCreateInfo = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		queueFamilyNumber,
		deviceQueueCreateInfo,
		0,
		VK_NULL_HANDLE,
		sizeof(deviceExtensions) / sizeof(deviceExtensions[0]),			// extension number
		deviceExtensions,	// extension names
		&physicalDeviceFeatures
	};

	VkDevice device;
	vkCreateDevice(physicalDevice, &deviceCreateInfo, VK_NULL_HANDLE, &device);

	for(uint32_t i = 0; i < queueFamilyNumber; i++){
		free(queuePriorities[i]);
	}
	free(queuePriorities);
	free(deviceQueueCreateInfo);
	return device;
}

void deleteDevice(VkDevice *pDevice){
	vkDestroyDevice(*pDevice, VK_NULL_HANDLE);
}
