#include "vk_fun.h"

static VkSemaphore *createSemaphores(const VkDevice device, const uint32_t maxFrames){
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0
	};

	VkSemaphore *semaphore = (VkSemaphore *)malloc(maxFrames * sizeof(VkSemaphore));
	for(uint32_t i = 0; i < maxFrames; i++){
		vkCreateSemaphore(device, &semaphoreCreateInfo, VK_NULL_HANDLE, &semaphore[i]);
	}
	return semaphore;
}

static void deleteSemaphores(const VkDevice device, VkSemaphore *semaphores, const uint32_t maxFrames){
	for(uint32_t i = 0; i < maxFrames; i++){
		vkDestroySemaphore(device, semaphores[i], VK_NULL_HANDLE);
	}
	free(semaphores);
}

static VkFence *createFences(const VkDevice device, const uint32_t maxFrames){
	VkFenceCreateInfo fenceCreateInfo = {
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		VK_NULL_HANDLE,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	VkFence *fence = (VkFence *)malloc(maxFrames * sizeof(VkFence));
	for(uint32_t i = 0; i < maxFrames; i++){
		vkCreateFence(device, &fenceCreateInfo, VK_NULL_HANDLE, &fence[i]);
	}
	return fence;
}

static void deleteFences(const VkDevice device, VkFence *fences, const uint32_t maxFrames){
	for(uint32_t i = 0; i < maxFrames; i++){
		vkDestroyFence(device, fences[i], VK_NULL_HANDLE);
	}
	free(fences);
}

//VkFence *createEmptyFences(uint32_t maxFrames){
//	VkFence *fence = (VkFence *)malloc(maxFrames * sizeof(VkFence));
//	for(uint32_t i = 0; i < maxFrames; i++){
//		fence[i] = VK_NULL_HANDLE;
//	}
//	return fence;
//}
//
//void deleteEmptyFences(VkFence **ppFences){
//	free(*ppFences);
//}

syncObjects createSyncObjects(const VkDevice device, const uint32_t maxFrames){
	syncObjects syncObjects;
	syncObjects.semaphores.signal = createSemaphores(device, maxFrames);
	syncObjects.semaphores.wait = createSemaphores(device, maxFrames);
	syncObjects.fences = createFences(device, maxFrames);
	return syncObjects;
}

void deleteSyncObjects(const VkDevice device, syncObjects *pSyncObjects, const uint32_t maxFrames){
	deleteSemaphores(device, pSyncObjects->semaphores.signal, maxFrames);
	deleteSemaphores(device, pSyncObjects->semaphores.wait, maxFrames);
	deleteFences(device, pSyncObjects->fences, maxFrames);
}