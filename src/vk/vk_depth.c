#include "vk_fun.h"

static VkFormat findSupportedFormat(const VkPhysicalDevice physicalDevice, const VkFormat *candidates, const uint32_t candidateCount, const VkImageTiling tiling, const VkFormatFeatureFlags features) {
    if (physicalDevice == VK_NULL_HANDLE || candidates == NULL || candidateCount == 0) {
        fprintf(stderr, "Invalid arguments to findSupportedFormat\n");
        return VK_FORMAT_UNDEFINED; // Return an undefined format as an error indicator
    }
    for (uint32_t i = 0; i < candidateCount; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return candidates[i];
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return candidates[i];
        }
    }
    fprintf(stderr, "failed to find supported format!\n");
    return VK_FORMAT_UNDEFINED; // Return an undefined format as an error indicator
}

VkFormat findDepthFormat(const VkPhysicalDevice physicalDevice) {
    VkFormat candidates[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    return findSupportedFormat(physicalDevice, candidates, sizeof(candidates) / sizeof(candidates[0]), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}


VkBool32 formatIsFilterable(const VkPhysicalDevice physicalDevice, const VkFormat format, const VkImageTiling tiling) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) == VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) {
        return VK_TRUE;
    } else if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) == VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) {
        return VK_TRUE;
    }
    printf("format is not filterable!\n");
    return VK_FALSE;
}