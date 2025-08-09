#include "vk_fun.h"

static VkPipelineLayout createPipelineLayout(const VkDevice device, const VkDescriptorSetLayout *pDescriptorSetLayout){
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = pDescriptorSetLayout,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = VK_NULL_HANDLE
	};

	VkPipelineLayout pipelineLayout;
	vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &pipelineLayout);
	return pipelineLayout;
}

static VkPipelineLayout createOffScrenePipelineLayout(const VkDevice device, const VkDescriptorSetLayout *pDescriptorSetLayout){
	VkPushConstantRange pushConstantRange = {
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.offset = 0,
		.size = sizeof(float[4][4])
	};
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = pDescriptorSetLayout,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pushConstantRange
	};

	VkPipelineLayout pipelineLayout;
	vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &pipelineLayout);
	return pipelineLayout;
}

static void deletePipelineLayout(const VkDevice device, VkPipelineLayout *pPipelineLayout){
	vkDestroyPipelineLayout(device, *pPipelineLayout, VK_NULL_HANDLE);
}

static VkPipelineShaderStageCreateInfo configureShaderStageCreateInfo(const VkShaderModule shaderModule, const VkShaderStageFlagBits stage, const char *entryName){
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = stage,
		.module = shaderModule,
		.pName = entryName
	};
	return shaderStageCreateInfo;
}

static VkVertexInputBindingDescription *getBindingDescriptions(uint32_t *pBindingNum) {
	*pBindingNum = 1;
    VkVertexInputBindingDescription *bindingDescription = malloc(*pBindingNum * sizeof(VkVertexInputBindingDescription));
	bindingDescription[0] = (VkVertexInputBindingDescription){
        .binding = 0,
        .stride = sizeof(vertex_t),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
    return bindingDescription;
}

static VkVertexInputAttributeDescription *getAttributeDescriptions(uint32_t *pAttributeNum) {
	*pAttributeNum = 3;
    VkVertexInputAttributeDescription *attributeDescriptions = malloc(*pAttributeNum * sizeof(VkVertexInputAttributeDescription));
    attributeDescriptions[0] = (VkVertexInputAttributeDescription){
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(vertex_t, pos)
    };
    attributeDescriptions[1] = (VkVertexInputAttributeDescription){
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(vertex_t, color)
    };
	attributeDescriptions[2] = (VkVertexInputAttributeDescription){
		.location = 2,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(vertex_t, normal)
	};
    return attributeDescriptions;
}


static VkPipelineVertexInputStateCreateInfo configureVertexInputStateCreateInfo(const VkVertexInputBindingDescription *bindingDescriptions, const uint32_t bindNum, const VkVertexInputAttributeDescription *attributeDescriptions, const uint32_t attributeNum) {

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = bindNum,
        .pVertexBindingDescriptions = bindingDescriptions,
        .vertexAttributeDescriptionCount = attributeNum,
        .pVertexAttributeDescriptions = attributeDescriptions,
    };
    return vertexInputStateCreateInfo;
}

static VkPipelineInputAssemblyStateCreateInfo configureInputAssemblyStateCreateInfo() {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.flags = 0,
		.pNext = VK_NULL_HANDLE,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    return inputAssemblyStateCreateInfo;
}

static VkPipelineViewportStateCreateInfo configureViewportStateCreateInfo() {
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = VK_NULL_HANDLE,
        .scissorCount = 1,
        .pScissors = VK_NULL_HANDLE,
		.flags = 0,
		.pNext = VK_NULL_HANDLE
    };

    return viewportStateCreateInfo;
}

static VkPipelineRasterizationStateCreateInfo configureRasterizationStateCreateInfo() {
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    	.pNext = NULL,
    	.flags = 0,
    	.depthClampEnable = VK_FALSE,
    	.rasterizerDiscardEnable = VK_FALSE,
    	.polygonMode = VK_POLYGON_MODE_FILL,
    	.cullMode = VK_CULL_MODE_BACK_BIT,
    	.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    	.depthBiasEnable = VK_FALSE,
    	.depthBiasConstantFactor = 0.0f,
    	.depthBiasClamp = 0.0f,
    	.depthBiasSlopeFactor = 0.0f,
    	.lineWidth = 1.0f // Default line width
    };

    return rasterizationStateCreateInfo;
}

static VkPipelineMultisampleStateCreateInfo configureMultisampleStateCreateInfo(const VkSampleCountFlagBits numSamples){
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
		.flags = 0,
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = numSamples,
		.minSampleShading = 1.0f,
		.pSampleMask = VK_NULL_HANDLE,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};

	return multisampleStateCreateInfo;
}

static VkPipelineColorBlendAttachmentState configureColorBlendAttachmentState() {
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = 0xf
    };

    return colorBlendAttachmentState;
}

static VkPipelineColorBlendStateCreateInfo configureColorBlendStateCreateInfo(const VkPipelineColorBlendAttachmentState *pColorBlendAttachmentState) {
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = pColorBlendAttachmentState,
        .blendConstants = {1.0f, 1.0f, 1.0f, 1.0f},
		.flags = 0,
		.pNext = VK_NULL_HANDLE
    };

    return colorBlendStateCreateInfo;
}

//static VkDynamicState *getDynamicState(){
//    VkDynamicState *dynamicStates = malloc(2 * sizeof(VkDynamicState));
//	dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
//	dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
//	return dynamicStates;
//}

static VkPipelineDynamicStateCreateInfo configureDynamicStateCreateInfo(const VkDynamicState *dynamicStates, const uint32_t dynamicStateCount){
	VkPipelineDynamicStateCreateInfo dynamicState;
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = dynamicStateCount;
	dynamicState.pDynamicStates = dynamicStates;
	dynamicState.pNext = VK_NULL_HANDLE;
	dynamicState.flags = 0;

	return dynamicState;
}

static VkPipelineDepthStencilStateCreateInfo configureDepthStencilStateCreateInfo(){
	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
		.depthBoundsTestEnable = VK_FALSE,
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 0.0f,
		.stencilTestEnable = VK_FALSE,
		.back = {
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.depthFailOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.compareMask = 0,
			.writeMask = 0,
			.reference = 0
		},
		.front = {
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.depthFailOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.compareMask = 0,
			.writeMask = 0,
			.reference = 0
		}
	};
	return depthStencilStateCreateInfo;
}

static VkPipelineCache createPipelineCache(const VkDevice device){
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.initialDataSize = 0,
		.pInitialData = VK_NULL_HANDLE
	};

	VkPipelineCache pipelineCache;
	vkCreatePipelineCache(device, &pipelineCacheCreateInfo, VK_NULL_HANDLE, &pipelineCache);
	return pipelineCache;
}

static void deletePipelineCache(const VkDevice device, VkPipelineCache *pPipelineCache){
	vkDestroyPipelineCache(device, *pPipelineCache, VK_NULL_HANDLE);
}


static void deletePipeline(const VkDevice device, VkPipeline *pPipeline){
	vkDestroyPipeline(device, *pPipeline, VK_NULL_HANDLE);
}


pipelines createPipelines(const VkDevice device, const VkRenderPass sceneRenderPass, const VkRenderPass offscreenRenderPass, const VkSampleCountFlagBits numSamples, const VkDescriptorSetLayout *pDescriptorSetLayout, const VkExtent2D sceneExtent, const uint32_t shadowMapResolution){
	pipelines pipes;
	pipes.scene.layout = createPipelineLayout(device, pDescriptorSetLayout);
	pipes.offscreen.layout = createOffScrenePipelineLayout(device, pDescriptorSetLayout);
	VkPipelineCache pipelineCache = createPipelineCache(device);
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = configureInputAssemblyStateCreateInfo();
	VkPipelineRasterizationStateCreateInfo rasterization = configureRasterizationStateCreateInfo();
	VkPipelineColorBlendAttachmentState blendAttachment = configureColorBlendAttachmentState();
	VkPipelineColorBlendStateCreateInfo blendState = configureColorBlendStateCreateInfo(&blendAttachment);
	VkPipelineDepthStencilStateCreateInfo depthStencil = configureDepthStencilStateCreateInfo();
	VkPipelineViewportStateCreateInfo viewportState = configureViewportStateCreateInfo();
	VkPipelineMultisampleStateCreateInfo multisample = configureMultisampleStateCreateInfo(numSamples);
	VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_BIAS};
	VkPipelineDynamicStateCreateInfo dynamicState = configureDynamicStateCreateInfo(dynamicStates, 2);
	uint32_t bindNum, attributeNum;
	VkVertexInputBindingDescription *bindingDescriptions = getBindingDescriptions(&bindNum);
	VkVertexInputAttributeDescription *attributeDescriptions = getAttributeDescriptions(&attributeNum);
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = configureVertexInputStateCreateInfo(bindingDescriptions, bindNum, attributeDescriptions, attributeNum);
	VkPipelineShaderStageCreateInfo shaderStage[2];

	VkGraphicsPipelineCreateInfo pipelineCI = {
	    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .stageCount = 2,
	    .pStages = shaderStage,
	    .pInputAssemblyState = &inputAssembly,
	    .pViewportState = &viewportState,
	    .pRasterizationState = &rasterization,
	    .pMultisampleState = &multisample,
	    .pDepthStencilState = &depthStencil,
	    .pColorBlendState = &blendState,
	    .pDynamicState = &dynamicState,
	    .layout = pipes.scene.layout,
	    .renderPass = sceneRenderPass,
	    .subpass = 0,
		.pVertexInputState = &vertexInputStateCreateInfo,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};
	//scenepipeline
	shaderStage[0] = configureShaderStageCreateInfo(getShader(device, "shaders/scene.vert.spv"), VK_SHADER_STAGE_VERTEX_BIT, "main");
	shaderStage[1] = configureShaderStageCreateInfo(getShader(device, "shaders/scene.frag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT, "main");
	if(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, VK_NULL_HANDLE, &pipes.scene.pipe) != VK_SUCCESS){printf("failed to create graphics pipeline\n");exit(EXIT_FAILURE);}
	pipes.scene.scissor = configureScissor(sceneExtent);
	pipes.scene.viewport = configureViewport(sceneExtent);
	vkDestroyShaderModule(device, shaderStage[0].module, VK_NULL_HANDLE);
	vkDestroyShaderModule(device, shaderStage[1].module, VK_NULL_HANDLE);
	//offscreen pipeline
	shaderStage[0] = configureShaderStageCreateInfo(getShader(device, "shaders/shadow.vert.spv"), VK_SHADER_STAGE_VERTEX_BIT, "main");
	shaderStage[1] = configureShaderStageCreateInfo(getShader(device, "shaders/shadow.frag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT, "main");
	pipelineCI.layout = pipes.offscreen.layout;
	rasterization.cullMode = VK_CULL_MODE_FRONT_BIT;
	//rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE;
	//rasterization.depthBiasEnable = VK_TRUE;
	//dynamicState.dynamicStateCount = 3;
	pipelineCI.renderPass = offscreenRenderPass;
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	if(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, VK_NULL_HANDLE, &pipes.offscreen.pipe) != VK_SUCCESS){printf("failed to create graphics pipeline\n");exit(EXIT_FAILURE);}
	pipes.offscreen.scissor = configureScissor((VkExtent2D){shadowMapResolution, shadowMapResolution});
	pipes.offscreen.viewport = configureViewport((VkExtent2D){shadowMapResolution, shadowMapResolution});
	pipes.offscreen.bias = (depthBias){0.0f, 0.0f, 0.0f};
	vkDestroyShaderModule(device, shaderStage[0].module, VK_NULL_HANDLE);
	vkDestroyShaderModule(device, shaderStage[1].module, VK_NULL_HANDLE);
	//cleanup
	deletePipelineCache(device, &pipelineCache);
	free(bindingDescriptions);
	free(attributeDescriptions);
	return pipes;
}

void deletePipelines(const VkDevice device, pipelines *pPipelines){
	deletePipelineLayout(device, &pPipelines->offscreen.layout);
	deletePipelineLayout(device, &pPipelines->scene.layout);
	deletePipeline(device, &pPipelines->scene.pipe);
	deletePipeline(device, &pPipelines->offscreen.pipe);
}

VkViewport configureViewport(const VkExtent2D extent) {
	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)extent.width,
		.height = (float)extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	return viewport;
}

VkRect2D configureScissor(const VkExtent2D extent) {
    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = extent
    };
    return scissor;
}