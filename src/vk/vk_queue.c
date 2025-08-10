#include "vk_fun.h"

uint32_t getqueueFamilyNumber(const VkPhysicalDevice physicalDevice){
	uint32_t queueFamilyNumber = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyNumber, VK_NULL_HANDLE);
	return queueFamilyNumber;
}

VkQueueFamilyProperties *getQueueFamilyProperties(const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyNumber){
	VkQueueFamilyProperties *queueFamilyProperties = (VkQueueFamilyProperties *)malloc(queueFamilyNumber * sizeof(VkQueueFamilyProperties));
	uint32_t num = queueFamilyNumber;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &num, queueFamilyProperties);
	return queueFamilyProperties;
}

void deleteQueueFamilyProperties(VkQueueFamilyProperties **ppQueueFamilyProperties){
	free(*ppQueueFamilyProperties);
}

uint32_t getBestGraphicsQueueFamilyindex(const VkQueueFamilyProperties *pQueueFamilyProperties, const uint32_t queueFamilyNumber){
	uint32_t graphicsQueueFamilyNumber = 0;
	uint32_t *graphicsQueueFamilyIndices = (uint32_t *)malloc(queueFamilyNumber * sizeof(uint32_t));

	for(uint32_t i = 0; i < queueFamilyNumber; i++){
		if((pQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0){
			graphicsQueueFamilyIndices[graphicsQueueFamilyNumber] = i;
			graphicsQueueFamilyNumber++;
		}
	}

	uint32_t bestGraphicsQueueFamilyQueueCount = 0, bestGraphicsQueueFamilyIndex = 0;

	for(uint32_t i = 0; i < graphicsQueueFamilyNumber; i++){
		if(pQueueFamilyProperties[graphicsQueueFamilyIndices[i]].queueCount > bestGraphicsQueueFamilyQueueCount){
			bestGraphicsQueueFamilyQueueCount = pQueueFamilyProperties[graphicsQueueFamilyIndices[i]].queueCount;
			bestGraphicsQueueFamilyIndex = i;
		}
	}

	free(graphicsQueueFamilyIndices);
	return bestGraphicsQueueFamilyIndex;
}

static uint32_t getGraphicsQueueMode(const VkQueueFamilyProperties *pQueueFamilyProperties, const uint32_t graphicsQueueFamilyindex){
	if(pQueueFamilyProperties[graphicsQueueFamilyindex].queueCount == 1){
		return 0;
	}else if(pQueueFamilyProperties[graphicsQueueFamilyindex].queueCount > 1){
		return 1;
	}else{
		return 2;
	}
}

static VkQueue getDrawingQueue(const VkDevice device, const uint32_t graphicsQueueFamilyindex){
	VkQueue drawingQueue = VK_NULL_HANDLE;
	vkGetDeviceQueue(device, graphicsQueueFamilyindex, 0, &drawingQueue);
	return drawingQueue;
}

static VkQueue getPresentingQueue(const VkDevice device, const uint32_t graphicsQueueFamilyindex, const uint32_t graphicsQueueMode){
	VkQueue presentingQueue = VK_NULL_HANDLE;
	if(graphicsQueueMode == 0){
		vkGetDeviceQueue(device, graphicsQueueFamilyindex, 0, &presentingQueue);
	}else if(graphicsQueueMode == 1){
		vkGetDeviceQueue(device, graphicsQueueFamilyindex, 1, &presentingQueue);
	}
	return presentingQueue;
}

queueAttachment createQueueAttachment(const VkDevice device, const VkQueueFamilyProperties *queueFamilyProperties, const uint32_t bestGraphicsQueueFamilyindex){
	queueAttachment queue;
	queue.drawingMode = getGraphicsQueueMode(queueFamilyProperties, bestGraphicsQueueFamilyindex);
	queue.drawing = getDrawingQueue(device, bestGraphicsQueueFamilyindex);
	queue.presenting = getPresentingQueue(device, bestGraphicsQueueFamilyindex, queue.drawingMode);
	return queue;
}

VkPresentInfoKHR createPresentInfoKHR(const VkSemaphore *pWaitSemaphores, const VkSwapchainKHR *pSwapchain, const uint32_t *pImageIndex){
	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = VK_NULL_HANDLE,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = pWaitSemaphores,
		.swapchainCount = 1,
		.pSwapchains = pSwapchain,
		.pImageIndices = pImageIndex,
		.pResults = VK_NULL_HANDLE
	};
	return presentInfo;
}

VkSubmitInfo createSubmitInfo(const VkSemaphore *pWaitSemaphores, const VkCommandBuffer *pCommandBuffer, const VkSemaphore *pSignalSemaphores, const VkPipelineStageFlags *pPipelineStage){
	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = VK_NULL_HANDLE,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = pWaitSemaphores,
		.pWaitDstStageMask = pPipelineStage,
		.commandBufferCount = 1,
		.pCommandBuffers = pCommandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = pSignalSemaphores
	};
	return submitInfo;
}