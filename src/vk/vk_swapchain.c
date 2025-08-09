#include "vk_fun.h"

static uint32_t getSwapchainImageNumber(const VkDevice device, const VkSwapchainKHR swapchain){
	uint32_t swapchainImageNumber;
	vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageNumber, VK_NULL_HANDLE);
	return swapchainImageNumber;
}

static VkImage *getSwapchainImages(const VkDevice device, const VkSwapchainKHR swapchain, const uint32_t swapchainImageNumber){
	VkImage *swapchainImages = (VkImage *)malloc(swapchainImageNumber * sizeof(VkImage));
	uint32_t imgNum = swapchainImageNumber;
	vkGetSwapchainImagesKHR(device, swapchain, &imgNum, swapchainImages);
	return swapchainImages;
}

//TODO:
static void deleteSwapchainImages(const VkDevice device, VkImage *images, const uint32_t imageNumber){
	for(uint32_t i = 0; i < imageNumber; i++){
		vkDestroyImage(device, images[i], VK_NULL_HANDLE);
	}
	free(images);
}

static VkImageView *createImageViews(const VkDevice device, const VkImage *images, const VkSurfaceFormatKHR format, const uint32_t imageNumber, const uint32_t imageArrayLayers){
	VkImageView *imageViews = (VkImageView *)malloc(imageNumber * sizeof(VkImageView));

	for(uint32_t i = 0; i < imageNumber; i++){
		imageViews[i] = createImageView(device, images[i], format.format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, imageArrayLayers, 0);
	}
	return imageViews;
}

static void deleteImageViews(const VkDevice device, VkImageView *ImageViews, const uint32_t imageViewNumber){
	for(uint32_t i = 0; i < imageViewNumber; i++){
		vkDestroyImageView(device, ImageViews[i], VK_NULL_HANDLE);
	}
	free(ImageViews);
}

static VkSwapchainKHR createSwapChain(const VkDevice device, const VkSurfaceKHR surface, const VkSurfaceCapabilitiesKHR surfaceCapabilities, const VkSurfaceFormatKHR surfaceFormat, const VkExtent2D extent, const VkPresentModeKHR presentMode, const uint32_t imageArrayLayers, const uint32_t graphicsQueueMode){
	VkSharingMode imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	uint32_t queueFamilyIndexCount = 0, *pQueueFamilyIndices = VK_NULL_HANDLE;
	uint32_t queueFamilyIndices[] = {0, 1};
	if(graphicsQueueMode == 1){
		imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		queueFamilyIndexCount = 2;
		pQueueFamilyIndices = queueFamilyIndices;
	}
	
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.surface = surface,
		.minImageCount = surfaceCapabilities.minImageCount,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = imageArrayLayers,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = imageSharingMode,
		.queueFamilyIndexCount = queueFamilyIndexCount,
		.pQueueFamilyIndices = pQueueFamilyIndices,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = presentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};

	VkSwapchainKHR swapchain;
	vkCreateSwapchainKHR(device, &swapchainCreateInfo, VK_NULL_HANDLE, &swapchain);
	return swapchain;
}

static void deleteSwapchain(const VkDevice device, VkSwapchainKHR *pSwapchain){
	vkDestroySwapchainKHR(device, *pSwapchain, VK_NULL_HANDLE);
}

static VkSurfaceCapabilitiesKHR getSurfaceCapabilities(const VkSurfaceKHR surface, const VkPhysicalDevice physicalDevice){
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
	return surfaceCapabilities;
}

static VkSurfaceFormatKHR getBestSurfaceFormat(const VkSurfaceKHR surface, const VkPhysicalDevice physicalDevice){
	uint32_t surfaceFormatNumber = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatNumber, VK_NULL_HANDLE);
	VkSurfaceFormatKHR *surfaceFormats = (VkSurfaceFormatKHR *)malloc(surfaceFormatNumber * sizeof(VkSurfaceFormatKHR));
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatNumber, surfaceFormats);
	VkSurfaceFormatKHR bestSurfaceFormat = surfaceFormats[0];

	free(surfaceFormats);
	return bestSurfaceFormat;
}

static VkPresentModeKHR getBestPresentMode(const VkSurfaceKHR surface, const VkPhysicalDevice physicalDevice){
	uint32_t presentModeNumber = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeNumber, VK_NULL_HANDLE);
	VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeNumber * sizeof(VkPresentModeKHR));
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeNumber, presentModes);

	VkPresentModeKHR bestPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	for(uint32_t i = 0; i < presentModeNumber; i++){
		if(presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR){
			bestPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		}
	}

	free(presentModes);
	return bestPresentMode;
}

static VkExtent2D getBestSwapchainExtent(const VkSurfaceCapabilitiesKHR surfaceCapabilities, const GLFWwindow *window, const int FramebufferWidth, const int FramebufferHeight){

	VkExtent2D bestSwapchainExtent;

	if(surfaceCapabilities.currentExtent.width < FramebufferWidth){
		bestSwapchainExtent.width = surfaceCapabilities.currentExtent.width;
	}else{
		bestSwapchainExtent.width = FramebufferWidth;
	}

	if(surfaceCapabilities.currentExtent.height < FramebufferHeight){
		bestSwapchainExtent.height = surfaceCapabilities.currentExtent.height;
	}else{
		bestSwapchainExtent.height = FramebufferHeight;
	}

	return bestSwapchainExtent;
}

swapchainAttachment createSwapchainAttachment(const VkDevice device, const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, GLFWwindow *pWindow, const uint32_t imageArrayLayers, const uint32_t graphicsQueueMode){
	swapchainAttachment attachment;
	VkPresentModeKHR presentMode = getBestPresentMode(surface, physicalDevice);
	int FramebufferWidth = 0, FramebufferHeight = 0;
	glfwGetFramebufferSize(pWindow, &FramebufferWidth, &FramebufferHeight);
	VkSurfaceCapabilitiesKHR surfaceCapabilities = getSurfaceCapabilities(surface, physicalDevice);

	attachment.extent = getBestSwapchainExtent(surfaceCapabilities, pWindow, FramebufferWidth, FramebufferHeight);
	attachment.surfaceFormat = getBestSurfaceFormat(surface, physicalDevice);
	attachment.swapchain = createSwapChain(device, surface, surfaceCapabilities, attachment.surfaceFormat, attachment.extent, presentMode, imageArrayLayers, graphicsQueueMode);
	attachment.imageNum = getSwapchainImageNumber(device, attachment.swapchain);
	attachment.images = getSwapchainImages(device, attachment.swapchain, attachment.imageNum);
	attachment.imageViews = createImageViews(device, attachment.images, attachment.surfaceFormat, attachment.imageNum, imageArrayLayers);
	return attachment;
}

void deleteSwapchainAttachment(const VkDevice device, swapchainAttachment *pAttachment){
	deleteImageViews(device, pAttachment->imageViews, pAttachment->imageNum);
	deleteSwapchain(device, &pAttachment->swapchain);
}

uint32_t acquireNextImage(const VkDevice device, const VkSwapchainKHR swapchain, const uint64_t timeout, const VkSemaphore semaphore, const VkFence fence){
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		printf("swap chain out of date!\n");
		exit(EXIT_FAILURE);
	} else if (result == VK_SUBOPTIMAL_KHR) {
		printf("swap chain suboptimal!\n");
		exit(EXIT_FAILURE);
    } else if (result != VK_SUCCESS) {
	    printf("failed to acquire swap chain image!\n");
		exit(EXIT_FAILURE);
	}
	return imageIndex;
}