#include "vk_fun.h"

VkSampleCountFlagBits getMaxUsableSampleCount(const VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

static uint32_t getPhysicalDeviceNumber(const VkInstance instance){
	uint32_t physicalDeviceNumber = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceNumber, VK_NULL_HANDLE);
	return physicalDeviceNumber;
}

static VkPhysicalDevice *getPhysicalDevices(const VkInstance instance, const uint32_t physicalDeviceNumber){
	VkPhysicalDevice *physicalDevices = (VkPhysicalDevice *)malloc(physicalDeviceNumber * sizeof(VkPhysicalDevice));
	uint32_t physDevNum = physicalDeviceNumber;
	vkEnumeratePhysicalDevices(instance, &physDevNum, physicalDevices);
	return physicalDevices;
}

static void deletePhysicalDevices(VkPhysicalDevice *physicalDevices){
	free(physicalDevices);
}

static uint32_t getPhysicalDeviceTotalMemory(const VkPhysicalDeviceMemoryProperties *pPhysicalDeviceMemoryProperties){
	uint32_t physicalDeviceTotalMemory = 0;
	for(int i = 0; i < (int)pPhysicalDeviceMemoryProperties->memoryHeapCount; i++){
		if((pPhysicalDeviceMemoryProperties->memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0){
			physicalDeviceTotalMemory += pPhysicalDeviceMemoryProperties->memoryHeaps[i].size;
		}
	}
	return physicalDeviceTotalMemory;
}

static uint32_t getBestPhysicalDeviceIndex(const VkPhysicalDevice *pPhysicalDevices, const uint32_t physicalDeviceNumber){
	VkPhysicalDeviceProperties *physicalDeviceProperties = (VkPhysicalDeviceProperties *)malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceProperties));
	VkPhysicalDeviceMemoryProperties *physicalDeviceMemoryProperties = (VkPhysicalDeviceMemoryProperties *)malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceMemoryProperties));
	VkPhysicalDeviceFeatures *physicalDeviceFeatures = (VkPhysicalDeviceFeatures *)malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceFeatures));

	uint32_t discreteGPUNumber = 0, integratedGPUNumber = 0;
	uint32_t *discreteGPUIndices = (uint32_t *)malloc(physicalDeviceNumber * sizeof(uint32_t));
	uint32_t *integratedGPUIndices = (uint32_t *)malloc(physicalDeviceNumber * sizeof(uint32_t));

	for(uint32_t i = 0; i < physicalDeviceNumber; i++){
		vkGetPhysicalDeviceProperties(pPhysicalDevices[i], &physicalDeviceProperties[i]);
		vkGetPhysicalDeviceMemoryProperties(pPhysicalDevices[i], &physicalDeviceMemoryProperties[i]);
		vkGetPhysicalDeviceFeatures(pPhysicalDevices[i], &physicalDeviceFeatures[i]);

		if(physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& physicalDeviceFeatures[i].samplerAnisotropy == VK_TRUE){
			discreteGPUIndices[discreteGPUNumber] = i;
			discreteGPUNumber++;
		}
		if(physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
		&& physicalDeviceFeatures[i].samplerAnisotropy == VK_TRUE){
			integratedGPUIndices[integratedGPUNumber] = i;
			integratedGPUNumber++;
		}
	}

	uint32_t bestPhysicalDeviceIndex = 0;
	VkDeviceSize bestPhysicalDeviceMemory = 0;

	if(discreteGPUNumber != 0){
		for(uint32_t i = 0; i < discreteGPUNumber; i++){
			if(bestPhysicalDeviceMemory < getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[discreteGPUIndices[i]])){
				bestPhysicalDeviceMemory = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[discreteGPUIndices[i]]);
				bestPhysicalDeviceIndex = discreteGPUIndices[i];
			}
		}
	}else if(integratedGPUNumber != 0){
		for(uint32_t i = 0; i < integratedGPUNumber; i++){
			if(bestPhysicalDeviceMemory < getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[integratedGPUIndices[i]])){
				bestPhysicalDeviceMemory = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[integratedGPUIndices[i]]);
				bestPhysicalDeviceIndex = integratedGPUIndices[i];
			}
		}
	}

	free(discreteGPUIndices);
	free(integratedGPUIndices);
	free(physicalDeviceMemoryProperties);
	free(physicalDeviceProperties);
	free(physicalDeviceFeatures);

	return bestPhysicalDeviceIndex;
}

VkPhysicalDevice getBestPhysicalDevice(const VkInstance instance){
	uint32_t physicalDeviceNumber = getPhysicalDeviceNumber(instance);
	VkPhysicalDevice *physicalDevices = getPhysicalDevices(instance, physicalDeviceNumber);
	uint32_t bestPhysicalDeviceIndex = getBestPhysicalDeviceIndex(physicalDevices, physicalDeviceNumber);
	VkPhysicalDevice bestPhysicalDevice = physicalDevices[bestPhysicalDeviceIndex];
	deletePhysicalDevices(physicalDevices);
	return bestPhysicalDevice;
}
