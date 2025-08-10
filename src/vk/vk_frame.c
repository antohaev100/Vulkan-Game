#include "vk_fun.h"

static VkRenderPass createSceneRenderPass(const VkDevice device, const VkFormat format, const VkFormat depthFormat, const VkSampleCountFlagBits numSamples){
	VkAttachmentDescription attachmentDescriptions[] = {
		{	// color attachment
			.flags = 0,
			.format = format,
			.samples = numSamples,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		},
		{	// depth attachment
			.flags = 0,
			.format = depthFormat,
			.samples = numSamples,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		},
		{	//colorAttachmentResolve
			.flags = 0,
			.format = format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		}
	};


	VkAttachmentReference colorAttachmentReference = {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depthAttachmentReference = {
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference colorAttachmentResolveReference = {
		2,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpassDescription = {
	    .flags = 0,
	    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
	    .inputAttachmentCount = 0,
	    .pInputAttachments = NULL,
	    .colorAttachmentCount = 1,
	    .pColorAttachments = &colorAttachmentReference,
	    .pResolveAttachments = &colorAttachmentResolveReference,
	    .pDepthStencilAttachment = &depthAttachmentReference,
	    .preserveAttachmentCount = 0,
		.pPreserveAttachments = NULL
	};

    VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	};

	VkRenderPassCreateInfo renderPassCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.attachmentCount = sizeof(attachmentDescriptions) / sizeof(attachmentDescriptions[0]), 
		.pAttachments = attachmentDescriptions,
		.subpassCount = 1,
		.pSubpasses = &subpassDescription,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	VkRenderPass renderPass;
	vkCreateRenderPass(device, &renderPassCreateInfo, VK_NULL_HANDLE, &renderPass);
	return renderPass;
}

static void deleteRenderPass(const VkDevice device, VkRenderPass *pRenderPass){
	vkDestroyRenderPass(device, *pRenderPass, VK_NULL_HANDLE);
}

static VkClearValue *configureClearValues(const VkClearColorValue color, const VkClearDepthStencilValue depth){
	VkClearValue *clearValues = malloc(2 * sizeof(VkClearValue));
	clearValues[0].color = color;
	clearValues[1].depthStencil = depth;
	return clearValues;
}

static void deleteClearValues(VkClearValue *clearValues){
	free(clearValues);
}

static VkRenderPassBeginInfo *configureRenderPassBeginInfo(const VkRenderPass renderPass, const VkFramebuffer *framebuffers, const uint32_t imageViewNumber, const VkExtent2D extent, const VkClearValue *clearValues, const uint32_t clearValueCount){
	VkRenderPassBeginInfo *infos = malloc(imageViewNumber * sizeof(VkRenderPassBeginInfo));
	for(uint32_t i = 0; i < imageViewNumber; i++){
		infos[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		infos[i].pNext = VK_NULL_HANDLE;
		infos[i].renderPass = renderPass;
		infos[i].framebuffer = framebuffers[i];
		infos[i].renderArea.offset = (VkOffset2D){0, 0};
		infos[i].renderArea.extent = extent;
		infos[i].clearValueCount = clearValueCount;
		infos[i].pClearValues = clearValues;
	}
	return infos;
}

static void deleteRenderPassBeginInfos(VkRenderPassBeginInfo *infos){
	free(infos);
}

static VkFramebuffer *createFramebuffers(const VkDevice device, const VkRenderPass renderPass, const VkExtent2D extent, const VkImageView *imageViews, const uint32_t imageViewNumber, const VkImageView depthImageView, const VkImageView colorImageView){
	VkFramebufferCreateInfo *framebufferCreateInfo = (VkFramebufferCreateInfo *)malloc(imageViewNumber * sizeof(VkFramebufferCreateInfo));
	VkFramebuffer *framebuffers = (VkFramebuffer *)malloc(imageViewNumber * sizeof(VkFramebuffer));

	for(uint32_t i = 0; i < imageViewNumber; i++){
		VkImageView attachments[] = {
			colorImageView,
			depthImageView,
			imageViews[i]
		};
		framebufferCreateInfo[i].sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo[i].pNext = VK_NULL_HANDLE;
		framebufferCreateInfo[i].flags = 0;
		framebufferCreateInfo[i].renderPass = renderPass;
		framebufferCreateInfo[i].attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
		framebufferCreateInfo[i].pAttachments = attachments;
		framebufferCreateInfo[i].width = extent.width;
		framebufferCreateInfo[i].height = extent.height;
		framebufferCreateInfo[i].layers = 1;

		vkCreateFramebuffer(device, &framebufferCreateInfo[i], VK_NULL_HANDLE, &framebuffers[i]);
	}

	free(framebufferCreateInfo);
	return framebuffers;
}

static void deleteFramebuffers(const VkDevice device, VkFramebuffer *framebuffers, const uint32_t framebufferNumber){
	for(uint32_t i = 0; i < framebufferNumber; i++){
		vkDestroyFramebuffer(device, framebuffers[i], VK_NULL_HANDLE);
	}
	free(framebuffers);
}

sceneRenderPassAttachment createScenePass(const VkDevice device, const VkPhysicalDevice physicalDevice, const VkFormat surfaceFormat, const VkFormat depthFormat, const VkSampleCountFlagBits numSamples, const VkExtent2D extent, const VkImageView *swapchainImageViews, const uint32_t imageViewNumber){
	sceneRenderPassAttachment pass;
	pass.renderPass = createSceneRenderPass(device, surfaceFormat, depthFormat, numSamples);
	pass.color = createFrameBufferAttachment(device, physicalDevice, extent.width, extent.height, surfaceFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, numSamples, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, VK_IMAGE_VIEW_TYPE_2D);
	pass.depth = createFrameBufferAttachment(device, physicalDevice, extent.width, extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, numSamples, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 0, VK_IMAGE_VIEW_TYPE_2D);
	pass.frameBuffers = createFramebuffers(device, pass.renderPass, extent, swapchainImageViews, imageViewNumber, pass.depth.view, pass.color.view);
	pass.clearValues = configureClearValues((VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}}, (VkClearDepthStencilValue){1.0f, 0});
	pass.beginInfos = configureRenderPassBeginInfo(pass.renderPass, pass.frameBuffers, imageViewNumber, extent, pass.clearValues, 2);
	return pass;
}

void deleteScenePass(const VkDevice device, sceneRenderPassAttachment *pPass, const uint32_t imageViewNumber){
	deleteRenderPass(device, &pPass->renderPass);
	deleteFrameBufferAttachment(device, &pPass->color);
	deleteFrameBufferAttachment(device, &pPass->depth);
	deleteFramebuffers(device, pPass->frameBuffers, imageViewNumber);
	deleteClearValues(pPass->clearValues);
	deleteRenderPassBeginInfos(pPass->beginInfos);
}

static VkRenderPass createOffScreenRenderPass(const VkDevice device, const VkFormat depthFormat, const VkFormat colorFormat){
	VkAttachmentDescription attachmentDescriptions[2] = {
		{
			.flags = 0,
			.format = colorFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		},
		{
			.flags = 0,
			.format = depthFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		}
	};
	
	VkAttachmentReference colorAttachmentReference = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depthAttachmentReference = {
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpassDescription = {
	    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
	    .colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentReference,
	    .pDepthStencilAttachment = &depthAttachmentReference,
		.inputAttachmentCount = 0,
		.pInputAttachments = NULL,
		.pResolveAttachments = NULL,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = NULL
	};

	VkRenderPassCreateInfo renderPassCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = sizeof(attachmentDescriptions) / sizeof(attachmentDescriptions[0]),
		.pAttachments = attachmentDescriptions,
		.subpassCount = 1,
		.pSubpasses = &subpassDescription,
		.dependencyCount = 0,
		.pDependencies = VK_NULL_HANDLE,
		.flags = 0
	};
	VkRenderPass renderPass;
	if(vkCreateRenderPass(device, &renderPassCreateInfo, VK_NULL_HANDLE, &renderPass) != VK_SUCCESS){fprintf(stderr, "Failed to create render pass\n");exit(EXIT_FAILURE);}
	return renderPass;
}

static VkFramebuffer *createOffScreenFrameBuffers(const VkDevice device, const VkRenderPass renderPass, const VkImageView depth, const VkImageView *shadowCubeMapFaceImageViews, const uint32_t shadowMapResolution){
	VkImageView attachments[2];
	attachments[1] = depth;
	VkFramebufferCreateInfo framebufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = renderPass,
		.attachmentCount = sizeof(attachments) / sizeof(attachments[0]),
		.pAttachments = attachments,
		.width = shadowMapResolution,
		.height = shadowMapResolution,
		.layers = 1
	};
	VkFramebuffer *frameBuffers = malloc(6 * sizeof(VkFramebuffer));
	for(int i = 0; i < 6; i++){
		attachments[0] = shadowCubeMapFaceImageViews[i];
		if(vkCreateFramebuffer(device, &framebufferCreateInfo, VK_NULL_HANDLE, &frameBuffers[i]) != VK_SUCCESS){fprintf(stderr, "Failed to create frame buffer\n");exit(EXIT_FAILURE);}
	}
	return frameBuffers;
}

static void deleteOffScreenFrameBuffers(const VkDevice device, VkFramebuffer *frameBuffers){
	for(int i = 0; i < 6; i++){
		vkDestroyFramebuffer(device, frameBuffers[i], VK_NULL_HANDLE);
	}
	free(frameBuffers);
}

//static VkRenderPassBeginInfo createOffScreenRenderPassBeginInfo(const VkRenderPass renderPass, const VkFramebuffer frameBuffer, const uint32_t shadowMapResolution, const VkClearValue *pClearValue){
//	VkRenderPassBeginInfo beginInfo = {
//		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
//		.renderPass = renderPass,
//		.framebuffer = frameBuffer,
//		.renderArea = {
//			.offset = {0, 0},
//			.extent = {shadowMapResolution, shadowMapResolution}
//		},
//		.clearValueCount = 1,
//		.pClearValues = pClearValue
//	};
//	return beginInfo;
//}

offScreenRenderPassAttachment createOffScreenPass(const VkDevice device, const VkPhysicalDevice physicalDevice, const VkFormat depthFormat, const VkFormat colorFormat, const uint32_t shadowMapResolution, const VkCommandPool commandPool, const VkQueue drawingQueue){
	offScreenRenderPassAttachment pass;
	pass.shadowMap.color = createFrameBufferAttachment(device, physicalDevice, shadowMapResolution, shadowMapResolution, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_IMAGE_VIEW_TYPE_CUBE);
	transferImageLayout(device, commandPool, drawingQueue, pass.shadowMap.color.image.image, 6, VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	pass.shadowMap.ImageViews = createShadowCubeMapFaceImageViews(device, pass.shadowMap.color.image.image, colorFormat);
	pass.shadowMap.sampler = createSampler(device, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, VK_COMPARE_OP_NEVER);

	pass.renderPass = createOffScreenRenderPass(device, depthFormat, colorFormat);
	VkImageAspectFlagBits aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (depthFormat >= VK_FORMAT_D16_UNORM_S8_UINT) {
		aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	pass.depth = createFrameBufferAttachment(device, physicalDevice, shadowMapResolution, shadowMapResolution, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, aspectFlags, 1, 0, VK_IMAGE_VIEW_TYPE_2D);
	transferImageLayout(device, commandPool, drawingQueue, pass.depth.image.image, 1, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, aspectFlags);
	pass.frameBuffers = createOffScreenFrameBuffers(device, pass.renderPass, pass.depth.view, pass.shadowMap.ImageViews, shadowMapResolution);
	pass.clearValues = configureClearValues((VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}}, (VkClearDepthStencilValue){1.0f, 0});
	pass.beginInfos = configureRenderPassBeginInfo(pass.renderPass, pass.frameBuffers, 6, (VkExtent2D){shadowMapResolution, shadowMapResolution}, pass.clearValues, 2);
	return pass;
}

void deleteOffScreenPass(const VkDevice device, offScreenRenderPassAttachment *pPass){
	deleteFrameBufferAttachment(device, &pPass->shadowMap.color);
	deleteShadowCubeMapFaceImageViews(device, pPass->shadowMap.ImageViews);
	deleteRenderPass(device, &pPass->renderPass);
	deleteFrameBufferAttachment(device, &pPass->depth);
	deleteSampler(device, &pPass->shadowMap.sampler);
	deleteOffScreenFrameBuffers(device, pPass->frameBuffers);
	deleteRenderPassBeginInfos(pPass->beginInfos);
	deleteClearValues(pPass->clearValues);
}


