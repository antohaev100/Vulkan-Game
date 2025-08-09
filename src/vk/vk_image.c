#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "vk_fun.h"



VkImageView createImageView(const VkDevice device, const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags, const VkImageViewType viewType, const uint32_t layerCount, const uint32_t baseArrayLayer) {
    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = viewType,
        .format = format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A
        },
        .subresourceRange = {
            .aspectMask = aspectFlags,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = baseArrayLayer,
            .layerCount = layerCount
        },
        .flags = 0
    };
    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, VK_NULL_HANDLE, &imageView) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create texture image view\n");
        exit(EXIT_FAILURE);
    }

    return imageView;
}

static void deleteImageView(const VkDevice device, VkImageView *pImageView) {
    vkDestroyImageView(device, *pImageView, VK_NULL_HANDLE);
}

static VkImageandMemory createImage(const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkMemoryPropertyFlags properties, const VkSampleCountFlagBits numSamples, const VkImageCreateFlags flags, const uint32_t arrayLayers) {
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = width,
        .extent.height = height,
        .extent.depth = 1,
        .mipLevels = 1,
        .arrayLayers = arrayLayers,
        .format = format,
        .tiling = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = VK_NULL_HANDLE,
        .usage = usage,
        .samples = numSamples,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .flags = flags
    };
    VkImageandMemory imageandMemory;
    VkResult result = vkCreateImage(device, &imageInfo, VK_NULL_HANDLE, &imageandMemory.image);
    if(result != VK_SUCCESS){
        printf("failed to create image!\n");
        return imageandMemory;
    }
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, imageandMemory.image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice)
    };

    result = vkAllocateMemory(device, &allocInfo, VK_NULL_HANDLE, &imageandMemory.memory);
    if(result != VK_SUCCESS){
        printf("failed to allocate image memory!\n");
        return imageandMemory;
    }
    if(vkBindImageMemory(device, imageandMemory.image, imageandMemory.memory, 0) != VK_SUCCESS){
        printf("failed to bind image memory!\n");
    }
    return imageandMemory;
}

static void deleteImage(const VkDevice device, VkImageandMemory *pImageandMemory) {
    vkDestroyImage(device, pImageandMemory->image, VK_NULL_HANDLE);
    vkFreeMemory(device, pImageandMemory->memory, VK_NULL_HANDLE);
}

void transferImageLayout(const VkDevice device, const VkCommandPool commandPool, const VkQueue drawingQueue, const VkImage image, const uint32_t layerCount, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage, VkImageAspectFlags aspectMask) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = aspectMask,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = layerCount
        },
        .srcAccessMask = srcAccessMask,
        .dstAccessMask = dstAccessMask
    };
    vkCmdPipelineBarrier(
        commandBuffer, 
        sourceStage, destinationStage,
        0,
        0, VK_NULL_HANDLE, 
        0, VK_NULL_HANDLE, 
        1, &barrier
    );
    endSingleTimeCommands(device, &commandBuffer, commandPool, drawingQueue);
}

frameBufferAttachment createFrameBufferAttachment(const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkMemoryPropertyFlags properties, const VkSampleCountFlagBits numSamples, const VkImageAspectFlags aspectFlags, const uint32_t arrayLayers, const VkImageCreateFlags imageFlags, const VkImageViewType viewType) {
    frameBufferAttachment attachment;
    attachment.image = createImage(device, physicalDevice, width, height, format, tiling, usage, properties, numSamples, imageFlags, arrayLayers);
    attachment.view = createImageView(device, attachment.image.image, format, aspectFlags, viewType, arrayLayers, 0);
    return attachment;
}

void deleteFrameBufferAttachment(const VkDevice device, frameBufferAttachment *pAttachment) {
    deleteImageView(device, &pAttachment->view);
    deleteImage(device, &pAttachment->image);
}

VkSampler createSampler(const VkDevice device, const VkFilter filter, const VkSamplerAddressMode addressMode, const VkCompareOp compareOp) {
    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .flags = 0,
        .pNext = VK_NULL_HANDLE,
        .magFilter = filter,
        .minFilter = filter, //TODO:
        .addressModeU = addressMode,
        .addressModeV = addressMode,
        .addressModeW = addressMode,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 1.0f,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_TRUE,
        .compareOp = compareOp, // Updated for PCF
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .mipLodBias = 0.0f,
        .minLod = 0.0f,
        .maxLod = 1.0f,
    };
    VkSampler sampler;
    if (vkCreateSampler(device, &samplerInfo, VK_NULL_HANDLE, &sampler) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create texture sampler\n");
        exit(EXIT_FAILURE);
    }
    return sampler;
}

void deleteSampler(const VkDevice device, VkSampler *pSampler) {
    vkDestroySampler(device, *pSampler, VK_NULL_HANDLE);
}

VkImageView *createShadowCubeMapFaceImageViews(const VkDevice device, const VkImage image, const VkFormat format){
    VkImageView *imageViews = malloc(6 * sizeof(VkImageView));
    for (uint32_t i = 0; i < 6; i++) {
        imageViews[i] = createImageView(device, image, format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, i);
    }
    return imageViews;
}
void deleteShadowCubeMapFaceImageViews(const VkDevice device, VkImageView *pImageViews){
    for(uint32_t i = 0; i < 6; i++){
        deleteImageView(device, &pImageViews[i]);
    }
    free(pImageViews);
}



bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

//void transitionImageLayout(VkDevice *pDevice, VkCommandPool *pCommandPool, VkImage *pImage, VkQueue *pDrawingQueue, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
//    VkCommandBuffer commandBuffer = beginSingleTimeCommands(pDevice, pCommandPool);
//
//    VkImageMemoryBarrier barrier = {
//        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//        .oldLayout = oldLayout,
//        .newLayout = newLayout,
//        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//        .image = *pImage,
//        .subresourceRange = {
//            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//            .baseMipLevel = 0,
//            .levelCount = 1,
//            .baseArrayLayer = 0,
//            .layerCount = 1
//        }
//    };
//    VkPipelineStageFlags sourceStage;
//    VkPipelineStageFlags destinationStage;
//
//    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
//        barrier.srcAccessMask = 0;
//        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//    } else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
//        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//    } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
//        barrier.srcAccessMask = 0;
//        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//
//        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//        if (hasStencilComponent(format)) {
//            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//        }
//    } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
//        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//        sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
//        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//
//        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//        if (hasStencilComponent(format)) {
//            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//        }
//    } else {
//        printf("unsupported layout transition!\n");
//        return;
//    }
//
//    vkCmdPipelineBarrier(
//        commandBuffer, 
//        sourceStage, destinationStage,
//        0,
//        0, VK_NULL_HANDLE, 
//        0, VK_NULL_HANDLE, 
//        1, &barrier
//    );
//
//    endSingleTimeCommands(pDevice, &commandBuffer, pCommandPool, pDrawingQueue);
//}

//void copyBufferToImage(VkDevice *pDevice, VkCommandPool *pCommandPool, VkImage *pImage, VkQueue *pDrawingQueue, VkBuffer *pBuffer, uint32_t width, uint32_t height) {
//    VkCommandBuffer commandBuffer = beginSingleTimeCommands(pDevice, pCommandPool);
//
//    VkBufferImageCopy region = {
//        .bufferOffset = 0,
//        .bufferRowLength = 0,
//        .bufferImageHeight = 0,
//        .imageSubresource = {
//            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//            .mipLevel = 0,
//            .baseArrayLayer = 0,
//            .layerCount = 1
//        },
//        .imageOffset = {0, 0, 0},
//        .imageExtent = {width, height, 1}
//    };
//    vkCmdCopyBufferToImage(
//        commandBuffer, 
//        *pBuffer, 
//        *pImage, 
//        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
//        1, 
//        &region
//    );
//    endSingleTimeCommands(pDevice, &commandBuffer, pCommandPool, pDrawingQueue);
//}




//VkImageandMemory createTextureImage(VkDevice *pDevice, VkPhysicalDevice *pPhysicalDevice, VkCommandPool *pCommandPool, VkQueue *pDrawingQueue) {
//    int texWidth, texHeight, texChannels;
//    stbi_uc* pixels = stbi_load("textures/statue.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//    VkDeviceSize imageSize = texWidth * texHeight * 4;
//
//    if (!pixels) {
//        printf("failed to load texture image!\n");
//    }
//
//    VkBufferandMemory stagingBufferandMemory = createBuffer(pDevice, pPhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//    void* data;
//    vkMapMemory(*pDevice, stagingBufferandMemory.memory, 0, imageSize, 0, &data);
//        memcpy(data, pixels, (size_t)imageSize);
//    vkUnmapMemory(*pDevice, stagingBufferandMemory.memory);
//    stbi_image_free(pixels);
//
//    VkImageandMemory imageandMemory= createImage(pDevice, pPhysicalDevice, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);
//    transitionImageLayout(pDevice, pCommandPool, &imageandMemory.image, pDrawingQueue, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//    copyBufferToImage(pDevice, pCommandPool, &imageandMemory.image, pDrawingQueue, &stagingBufferandMemory.buffer, texWidth, texHeight);
//    transitionImageLayout(pDevice, pCommandPool, &imageandMemory.image, pDrawingQueue, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//    deleteBuffer(pDevice, &stagingBufferandMemory);
//    return imageandMemory;
//}


