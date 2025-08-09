#include "vk_fun.h"


uint32_t findMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties, const VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    fprintf(stderr, "failed to find suitable memory type!\n");
    exit(EXIT_FAILURE);
}

static VkBufferCreateInfo getBufferCreateInfo(const uint32_t bufferSize, const VkBufferUsageFlags usage) {
    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .size = bufferSize,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = VK_NULL_HANDLE
    };
    return bufferCreateInfo;
}

static VkBufferandMemory createBuffer(const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t bufferSize, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties) {
    VkBufferandMemory bufferandMemory;
    VkBufferCreateInfo bufferCreateInfo = getBufferCreateInfo(bufferSize, usage);
    //printf("Buffer size: %d\n", bufferSize);
    VkResult result = vkCreateBuffer(device, &bufferCreateInfo, VK_NULL_HANDLE, &bufferandMemory.buffer);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create vertex buffer\n");
        exit(EXIT_FAILURE);
    }
    
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, bufferandMemory.buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties, physicalDevice)
    };

    result = vkAllocateMemory(device, &memoryAllocateInfo, VK_NULL_HANDLE, &bufferandMemory.memory);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to allocate vertex buffer memory\n");
        exit(EXIT_FAILURE);
    }

    vkBindBufferMemory(device, bufferandMemory.buffer, bufferandMemory.memory, 0);
    return bufferandMemory;
}

static void copyBuffer(VkBuffer *pDstBuffer, const VkBuffer srcBuffer, const VkDeviceSize size, const VkDevice device, const VkQueue graphicsQueue, const VkCommandPool commandPool) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size
    };
    vkCmdCopyBuffer(commandBuffer, srcBuffer, *pDstBuffer, 1, &copyRegion);
    endSingleTimeCommands(device, &commandBuffer, commandPool, graphicsQueue);
}

//TODO:
static VkBufferandMemory createStaticBuffer(const void *pData, const VkDevice device, const VkPhysicalDevice physicalDevice, const VkQueue graphicsQueue, const VkCommandPool commandPool, const uint32_t bufferSize, const VkBufferUsageFlags usage) {
    VkBufferandMemory staging = createBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* tempData;
    vkMapMemory(device, staging.memory, 0, bufferSize, 0, &tempData);
        memcpy(tempData, pData, (size_t) bufferSize);
    vkUnmapMemory(device, staging.memory);

    VkBufferandMemory special = createBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    copyBuffer(&special.buffer, staging.buffer, bufferSize, device, graphicsQueue, commandPool);
    vkDestroyBuffer(device, staging.buffer, VK_NULL_HANDLE);
    vkFreeMemory(device, staging.memory, VK_NULL_HANDLE);

    return special;
}

void deleteBuffer(const VkDevice device, VkBufferandMemory *pBufferandMemory) {
    vkDestroyBuffer(device, pBufferandMemory->buffer, VK_NULL_HANDLE);
    vkFreeMemory(device, pBufferandMemory->memory, VK_NULL_HANDLE);
}

mappedBuffer *createSceneUniformBuffers(const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t maxFrames) {
    VkDeviceSize bufferSize = sizeof(uniformDataScene);
    mappedBuffer *uniformBuffers = malloc(maxFrames * sizeof(mappedBuffer));

    for (uint32_t i = 0; i < maxFrames; i++) {
        uniformBuffers[i].buffer = createBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkMapMemory(device, uniformBuffers[i].buffer.memory, 0, bufferSize, 0, &uniformBuffers[i].pMappedData);
    }
    return uniformBuffers;
}

mappedBuffer createOffScreenUniformBuffer(const VkDevice device, const VkPhysicalDevice physicalDevice) {
    VkDeviceSize bufferSize = sizeof(uniformDataOffscreen);
    mappedBuffer uniformBuffer;
    uniformBuffer.buffer = createBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkMapMemory(device, uniformBuffer.buffer.memory, 0, bufferSize, 0, &uniformBuffer.pMappedData);
    return uniformBuffer;
}

void deleteMappedBuffers(const VkDevice device, mappedBuffer *buffers, const uint32_t bufferNum) {
    for (uint32_t i = 0; i < bufferNum; i++) {
        vkUnmapMemory(device, buffers[i].buffer.memory);
        deleteBuffer(device, &buffers[i].buffer);
    }
    free(buffers);
}


//TODO:
dynamicBuffers createDynamicBuffers(const VkDevice device, const VkPhysicalDevice physicalDevice, const vec indices, const vec vertices, const VkQueue graphicsQueue, const VkCommandPool commandPool, const uint32_t frameNum){
    dynamicBuffers buffers;
    buffers.buffers = malloc(frameNum * sizeof(vertexAndIndexBuffers));
    buffers.staging = malloc(frameNum * sizeof(stagingBufferAttachment));

    for(int i = 0; i < frameNum; i++){
        //staging buffers
        buffers.staging[i].vertexBufferSize = vertices.c * vertices.elemSize;
        buffers.staging[i].indexBufferSize = indices.c * indices.elemSize;
        buffers.staging[i].vertex.buffer = createBuffer(device, physicalDevice, buffers.staging[i].vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkMapMemory(device, buffers.staging[i].vertex.buffer.memory, 0, buffers.staging[i].vertexBufferSize, 0, &buffers.staging[i].vertex.pMappedData);
        buffers.staging[i].index.buffer = createBuffer(device, physicalDevice, buffers.staging[i].indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkMapMemory(device, buffers.staging[i].index.buffer.memory, 0, buffers.staging[i].indexBufferSize, 0, &buffers.staging[i].index.pMappedData);
        memcpy(buffers.staging[i].vertex.pMappedData, vertices.array, vertices.n * vertices.elemSize);
        memcpy(buffers.staging[i].index.pMappedData, indices.array, indices.n * indices.elemSize);
        //buffers
        buffers.buffers[i].vertexBufferSize = vertices.c * vertices.elemSize;
        buffers.buffers[i].indexBufferSize = indices.c * indices.elemSize;
        buffers.buffers[i].vertex = createBuffer(device, physicalDevice, buffers.buffers[i].vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        buffers.buffers[i].index = createBuffer(device, physicalDevice, buffers.buffers[i].indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        copyBuffer(&buffers.buffers[i].vertex.buffer, buffers.staging[i].vertex.buffer.buffer, buffers.buffers[i].vertexBufferSize, device, graphicsQueue, commandPool);
        copyBuffer(&buffers.buffers[i].index.buffer, buffers.staging[i].index.buffer.buffer, buffers.buffers[i].indexBufferSize, device, graphicsQueue, commandPool);
    }
    return buffers;
}

void deleteDynamicBuffers(const VkDevice device, dynamicBuffers *pBuffers, const uint32_t frameNum){
    for(int i = 0; i < frameNum; i++){
        vkUnmapMemory(device, pBuffers->staging[i].index.buffer.memory);
        deleteBuffer(device, &pBuffers->staging[i].index.buffer);
        vkUnmapMemory(device, pBuffers->staging[i].vertex.buffer.memory);
        deleteBuffer(device, &pBuffers->staging[i].vertex.buffer);
        deleteBuffer(device, &pBuffers->buffers[i].vertex);
        deleteBuffer(device, &pBuffers->buffers[i].index);
    }
    free(pBuffers->buffers);
    free(pBuffers->staging);
}

void updateDynamicBuffers(dynamicBuffers *pBuffers, const vec indices, const vec vertices, const VkCommandBuffer commandBuffer, const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t currentFrame){
    uint32_t indexSize = indices.c * indices.elemSize;
    uint32_t vertexSize = vertices.c * vertices.elemSize;

    if(pBuffers->staging[currentFrame].indexBufferSize != indexSize){
        //(printf("createing new staging index buffer for buffers %d, currently used buffers %d in current frame %d\n", currentFrame, pBuffers->preparedBufferIndex, currentFrame);
        pBuffers->staging[currentFrame].indexBufferSize = indexSize;
        vkUnmapMemory(device, pBuffers->staging[currentFrame].index.buffer.memory);
        deleteBuffer(device, &pBuffers->staging[currentFrame].index.buffer);
        pBuffers->staging[currentFrame].index.buffer = createBuffer(device, physicalDevice, indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkMapMemory(device, pBuffers->staging[currentFrame].index.buffer.memory, 0, indexSize, 0, &pBuffers->staging[currentFrame].index.pMappedData);
    }

    if(pBuffers->staging[currentFrame].vertexBufferSize != vertexSize){
        //printf("createing new staging vertex buffer\n");
        pBuffers->staging[currentFrame].vertexBufferSize = vertexSize;
        vkUnmapMemory(device, pBuffers->staging[currentFrame].vertex.buffer.memory);
        deleteBuffer(device, &pBuffers->staging[currentFrame].vertex.buffer);
        pBuffers->staging[currentFrame].vertex.buffer = createBuffer(device, physicalDevice, vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkMapMemory(device, pBuffers->staging[currentFrame].vertex.buffer.memory, 0, vertexSize, 0, &pBuffers->staging[currentFrame].vertex.pMappedData);

    }

    if(pBuffers->buffers[currentFrame].indexBufferSize != indexSize){
        //printf("createing new index buffer for %d, currently used buffer %d in current frame %d\n", currentFrame, pBuffers->preparedBufferIndex, currentFrame);
        pBuffers->buffers[currentFrame].indexBufferSize = indexSize;
        deleteBuffer(device, &pBuffers->buffers[currentFrame].index);
        pBuffers->buffers[currentFrame].index = createBuffer(device, physicalDevice, indexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    if(pBuffers->buffers[currentFrame].vertexBufferSize != vertexSize){
        //printf("createing new vertex buffer for %d, currently use buffer %d in current frame %d\n", currentFrame, pBuffers->preparedBufferIndex, currentFrame);
        pBuffers->buffers[currentFrame].vertexBufferSize = vertexSize;
        deleteBuffer(device, &pBuffers->buffers[currentFrame].vertex);
        pBuffers->buffers[currentFrame].vertex = createBuffer(device, physicalDevice, vertexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    memcpy(pBuffers->staging[currentFrame].vertex.pMappedData, vertices.array, vertices.n * vertices.elemSize);
    memcpy(pBuffers->staging[currentFrame].index.pMappedData, indices.array, indices.n * indices.elemSize);
    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
    };

    copyRegion.size = vertices.n * vertices.elemSize;
    vkCmdCopyBuffer(commandBuffer, pBuffers->staging[currentFrame].vertex.buffer.buffer, pBuffers->buffers[currentFrame].vertex.buffer, 1, &copyRegion);
    copyRegion.size = indices.n * indices.elemSize;
    vkCmdCopyBuffer(commandBuffer, pBuffers->staging[currentFrame].index.buffer.buffer, pBuffers->buffers[currentFrame].index.buffer, 1, &copyRegion);
}