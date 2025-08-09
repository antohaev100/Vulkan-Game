#include "vk_fun.h"

static VkDescriptorSetLayout createDescriptorSetLayout(const VkDevice device) {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags =  VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = VK_NULL_HANDLE
    };

    VkDescriptorSetLayoutBinding shadowCubeMapLayoutBinding = {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = VK_NULL_HANDLE
    };

    VkDescriptorSetLayoutBinding bindings[] = {uboLayoutBinding, shadowCubeMapLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = sizeof(bindings) / sizeof(bindings[0]),
        .pBindings = &bindings[0]
    };

    VkDescriptorSetLayout descriptorSetLayout;
    VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, VK_NULL_HANDLE, &descriptorSetLayout);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create descriptor set layout\n");
        exit(EXIT_FAILURE);
    }
    return descriptorSetLayout;
}

static void deleteDescriptorSetLayout(const VkDevice device, VkDescriptorSetLayout *pDescriptorSetLayout) {
    vkDestroyDescriptorSetLayout(device, *pDescriptorSetLayout, VK_NULL_HANDLE);
}

static VkDescriptorPool createDescriptorPool(const VkDevice device, const uint32_t maxFrames) {
    uint32_t maxSets = maxFrames + 1; //+1 for the offscreen descriptor
    VkDescriptorPoolSize poolSizes[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = maxSets
        },
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = maxSets
        }
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = sizeof(poolSizes) / sizeof(poolSizes[0]),
        .pPoolSizes = poolSizes,
        .maxSets = maxSets
    };

    VkDescriptorPool descriptorPool;
    VkResult result = vkCreateDescriptorPool(device, &poolInfo, VK_NULL_HANDLE, &descriptorPool);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create descriptor pool\n");
        exit(EXIT_FAILURE);
    }
    return descriptorPool;
}

static void deleteDescriptorPool(const VkDevice device, VkDescriptorPool *pDescriptorPool) {
    vkDestroyDescriptorPool(device, *pDescriptorPool, VK_NULL_HANDLE);
}

static descriptorSets createDescriptorSets(const VkDevice device, const uint32_t maxFrames, const VkDescriptorSetLayout *pDescriptorSetLayout, const VkDescriptorPool descriptorPool, const VkImageView shadowMapImageView, const VkSampler shadowMapSampler, const VkBuffer OffscreenBuffer, const VkBuffer *uniformBuffers) {
    descriptorSets sets = {
        .sceneSets = malloc((maxFrames) * sizeof(VkDescriptorSet)),
        .offscreen = VK_NULL_HANDLE
    };
   
    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = pDescriptorSetLayout
    };
    if(vkAllocateDescriptorSets(device, &allocInfo, &sets.offscreen) != VK_SUCCESS){fprintf(stderr, "Failed to allocate descriptor sets\n");exit(EXIT_FAILURE);}
    VkDescriptorBufferInfo offscreenBufferInfo = {
        .buffer = OffscreenBuffer,
        .offset = 0,
        .range = sizeof(uniformDataOffscreen)
    };
    VkWriteDescriptorSet offscreenDescriptorWrite = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = sets.offscreen,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .pBufferInfo = &offscreenBufferInfo
    };
    vkUpdateDescriptorSets(device, 1, &offscreenDescriptorWrite, 0, VK_NULL_HANDLE);

    VkDescriptorImageInfo shadowMapInfo = {
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .imageView = shadowMapImageView,
        .sampler = shadowMapSampler,
    };
    for (size_t i = 0; i < maxFrames; i++) {
        if(vkAllocateDescriptorSets(device, &allocInfo, &sets.sceneSets[i]) != VK_SUCCESS){fprintf(stderr, "Failed to allocate descriptor sets\n");exit(EXIT_FAILURE);}
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = uniformBuffers[i],
            .offset = 0,
            .range = sizeof(uniformDataScene)
        };
        VkWriteDescriptorSet descriptorWrite[] = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = sets.sceneSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = sets.sceneSets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pImageInfo = &shadowMapInfo
            }
        };
        vkUpdateDescriptorSets(device, sizeof(descriptorWrite) / sizeof(descriptorWrite[0]), descriptorWrite, 0, VK_NULL_HANDLE);
    }

    return sets;
}

static void deleteDescriptorSets(descriptorSets *pDescriptorSets) {
    free(pDescriptorSets->sceneSets);
}

descriptors createDescriptors(const VkDevice device, const uint32_t maxFrames, const VkImageView shadowMapImageView, const VkSampler shadowMapSampler, const VkBuffer OffscreenBuffer, const VkBuffer *uniformBuffers){
    descriptors descs;
    descs.layout = createDescriptorSetLayout(device);
    descs.pool = createDescriptorPool(device, maxFrames);
    descs.sets = createDescriptorSets(device, maxFrames, &descs.layout, descs.pool, shadowMapImageView, shadowMapSampler, OffscreenBuffer, uniformBuffers);
    return descs;
}

void deleteDescriptors(const VkDevice device, descriptors *pDescriptors){
    deleteDescriptorSets(&pDescriptors->sets);
    deleteDescriptorPool(device, &pDescriptors->pool);
    deleteDescriptorSetLayout(device, &pDescriptors->layout);
}