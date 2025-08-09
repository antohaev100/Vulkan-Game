#include "vk_fun.h"

static VkCommandPool createCommandPool(const VkDevice device, const uint32_t queueFamilyIndex){
	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		VK_NULL_HANDLE,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		queueFamilyIndex
	};

	VkCommandPool commandPool;
	vkCreateCommandPool(device, &commandPoolCreateInfo, VK_NULL_HANDLE, &commandPool);
	return commandPool;
}

static void deleteCommandPool(const VkDevice device, VkCommandPool *pCommandPool){
	vkDestroyCommandPool(device, *pCommandPool, VK_NULL_HANDLE);
}

static VkCommandBuffer *createCommandBuffers(const VkDevice device, const VkCommandPool commandPool, const uint32_t commandBufferNumber){
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = commandBufferNumber,
		.pNext = VK_NULL_HANDLE
	};

	VkCommandBuffer *commandBuffers = (VkCommandBuffer *)malloc(commandBufferNumber * sizeof(VkCommandBuffer));
	vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers);
	return commandBuffers;
}

static void deleteCommandBuffers(const VkDevice device, VkCommandBuffer *commandBuffers, const VkCommandPool commandPool, const uint32_t commandBufferNumber){
	vkFreeCommandBuffers(device, commandPool, commandBufferNumber, commandBuffers);
	free(commandBuffers);
}

static VkCommandBufferBeginInfo configureCommandBufferBeginInfo(){
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };
	return commandBufferBeginInfo;
}

VkCommandBuffer beginSingleTimeCommands(const VkDevice device, const VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandPool = commandPool,
		.commandBufferCount = 1
	};

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = NULL
	};

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void endSingleTimeCommands(const VkDevice device, VkCommandBuffer *pCommandBuffer, const VkCommandPool commandPool, const VkQueue drawingQueue) {
    vkEndCommandBuffer(*pCommandBuffer);

    VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = pCommandBuffer
	};

	vkQueueSubmit(drawingQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(drawingQueue);

	vkFreeCommandBuffers(device, commandPool, 1, pCommandBuffer);
}

commandAttachment createCommandAttachment(const VkDevice device, const uint32_t queueFamilyIndex, const uint32_t commandBufferNumber){
	commandAttachment command;
	command.pool = createCommandPool(device, queueFamilyIndex);
	command.buffers = createCommandBuffers(device, command.pool, commandBufferNumber);
	command.beginInfo = configureCommandBufferBeginInfo();
	return command;
}

void deleteCommandAttachment(const VkDevice device, commandAttachment *pCommand, const uint32_t commandBufferNumber){
	deleteCommandBuffers(device, pCommand->buffers, pCommand->pool, commandBufferNumber);
	deleteCommandPool(device, &pCommand->pool);
}