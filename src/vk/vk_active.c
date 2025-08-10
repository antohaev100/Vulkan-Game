#include "vk_fun.h"

//initilize filescope variables
static VkInstance instance;
static VkPhysicalDevice physicalDevice;
static VkDevice device;
static queueAttachment queue;
static VkSurfaceKHR surface;
static swapchainAttachment swapchain;
static VkFormat depthFormat;
static sceneRenderPassAttachment scenePass;
static offScreenRenderPassAttachment offScreenPass;
static pipelines pipes;
static commandAttachment command;
static uint32_t imageIndex;
static uint32_t currentFrame = 0;
static syncObjects sync;
static mappedBuffer *uniformBuffers;
static uniformDataScene uboScene;
static mappedBuffer uniformBufferOffscreen;
static uniformDataOffscreen uboOffscreen;
static descriptors descriptor;
static VkFormat depthFormat;
static VkSampleCountFlagBits msaaSamples;
static mapSize map;
static dynamicBuffers buffers;

static const uint32_t imageArrayLayers = 1;
static const uint32_t shadowMapResolution = 1024;

static vec vertices;
static vec indices;

static const float zNear = 0.9f;
static const float zFar = 10.1f;
static const float lightPOV = 90.0f;
static const float Up[] = {0.0f, 0.0f, 1.0f}; // Up direction for the light
static float lightPos[] = {0.00001f, 0.00001f, 9.0f, 1.0f}; // Position of the light source

static const VkDeviceSize offsets[] = {0};
static float viewMatrix[4][4];
static const float identity[4][4] = {
	{1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 1.0f}
};


static void updateCubeFace(uint32_t faceIndex){
	mat4_identity(viewMatrix);
	switch (faceIndex)
	{
	case 0: // POSITIVE_X
		mat4_rotate(viewMatrix, (const float(*)[4])viewMatrix, radians(90.0f), (float[]){0.0f, 1.0f, 0.0f});
		mat4_rotate(viewMatrix, (const float(*)[4])viewMatrix, radians(180.0f), (float[]){1.0f, 0.0f, 0.0f});
		break;
	case 1: // NEGATIVE_X
		mat4_rotate(viewMatrix, (const float(*)[4])viewMatrix, radians(-90.0f), (float[]){0.0f, 1.0f, 0.0f});
		mat4_rotate(viewMatrix, (const float(*)[4])viewMatrix, radians(180.0f), (float[]){1.0f, 0.0f, 0.0f});
		break;
	case 2: // POSITIVE_Y
		//mat4_rotate(viewMatrix, viewMatrix, radians(180.0f), (float[]){0.0f, 0.0f, 1.0f});
		mat4_rotate(viewMatrix, (const float(*)[4])viewMatrix, radians(90.0f), (float[]){1.0f, 0.0f, 0.0f});
		break;
	case 3: // NEGATIVE_Y
		//mat4_rotate(viewMatrix, viewMatrix, radians(180.0f), (float[]){0.0f, 0.0f, 1.0f});
		mat4_rotate(viewMatrix, (const float(*)[4])viewMatrix, radians(-90.0f), (float[]){1.0f, 0.0f, 0.0f});
		break;
	case 4: // POSITIVE_Z
		mat4_rotate(viewMatrix, (const float(*)[4])viewMatrix, radians(180.0f), (float[]){1.0f, 0.0f, 0.0f});
		break;
	case 5: // NEGATIVE_Z
		//viewMatrix[0][0] = -1.0f;
		mat4_rotate(viewMatrix, (const float(*)[4])viewMatrix, radians(180.0f), (float[]){0.0f, 0.0f, 1.0f});
		break;
	}
	vkCmdBeginRenderPass(command.buffers[currentFrame], &offScreenPass.beginInfos[faceIndex], VK_SUBPASS_CONTENTS_INLINE);
		vkCmdPushConstants(command.buffers[currentFrame], pipes.offscreen.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(viewMatrix), viewMatrix);
		vkCmdBindPipeline(command.buffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipes.offscreen.pipe);
		vkCmdBindDescriptorSets(command.buffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipes.offscreen.layout, 0, 1, &descriptor.sets.offscreen, 0, VK_NULL_HANDLE);
		vkCmdBindVertexBuffers(command.buffers[currentFrame], 0, 1, &buffers.buffers[currentFrame].vertex.buffer, offsets);
		vkCmdBindIndexBuffer(command.buffers[currentFrame], buffers.buffers[currentFrame].index.buffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(command.buffers[currentFrame], indices.n, 1, 0, 0, 0);
	vkCmdEndRenderPass(command.buffers[currentFrame]);
}

static void recordCommandBuffers(){
	//Reset command buffer
	vkResetCommandBuffer(command.buffers[currentFrame], 0);
	//Begin recording command buffer
	vkBeginCommandBuffer(command.buffers[currentFrame], &command.beginInfo);
		updateDynamicBuffers(&buffers, indices, vertices, command.buffers[currentFrame], device, physicalDevice, currentFrame);

		//draw shadowmap / offscreen pass (push view matrix)
		vkCmdSetViewport(command.buffers[currentFrame], 0, 1, &pipes.offscreen.viewport);
		vkCmdSetScissor(command.buffers[currentFrame], 0, 1, &pipes.offscreen.scissor);
		//vkCmdSetDepthBias(command.buffers[currentFrame], pipes.offscreen.bias.constant, pipes.offscreen.bias.clamp, pipes.offscreen.bias.slope);
		for (uint32_t face = 0; face < 6; face++) {
			updateCubeFace(face);
		}

		//Second pass: Scene rendering with applied shadow map
		vkCmdBeginRenderPass(command.buffers[currentFrame], &scenePass.beginInfos[imageIndex], VK_SUBPASS_CONTENTS_INLINE);
			vkCmdSetViewport(command.buffers[currentFrame], 0, 1, &pipes.scene.viewport);
			vkCmdSetScissor(command.buffers[currentFrame], 0, 1, &pipes.scene.scissor);
			vkCmdBindPipeline(command.buffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipes.scene.pipe);
			vkCmdBindVertexBuffers(command.buffers[currentFrame], 0, 1, &buffers.buffers[currentFrame].vertex.buffer, offsets);
			vkCmdBindIndexBuffer(command.buffers[currentFrame], buffers.buffers[currentFrame].index.buffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindDescriptorSets(command.buffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipes.scene.layout, 0, 1, &descriptor.sets.sceneSets[currentFrame], 0, VK_NULL_HANDLE);
			vkCmdDrawIndexed(command.buffers[currentFrame], indices.n, 1, 0, 0, 0);
		vkCmdEndRenderPass(command.buffers[currentFrame]);

	//End recording command buffer
	vkEndCommandBuffer(command.buffers[currentFrame]);
}

static void updateUniformBuffers(const sharedBuffer buffer) {
    // Update view matrix
    mat4_identity(uboScene.model);
    mat4_lookat(uboScene.view, buffer.cameraPos, buffer.cameraTarget, Up);
    mat4_perspective(uboScene.proj, radians(buffer.fov), swapchain.extent.width / (float)swapchain.extent.height, 0.1f, 50.0f);
    uboScene.proj[1][1] *= -1; // Invert the Y axis for Vulkan
    memcpy(uboScene.lightPos, lightPos, sizeof(lightPos));
	memcpy(uniformBuffers[imageIndex].pMappedData, &uboScene, sizeof(uboScene));
}

static void updateOffScreenUniformBuffer(){
	mat4_identity(uboOffscreen.view);
	mat4_perspective(uboOffscreen.proj, radians(lightPOV), 1.0f, zNear, zFar);
    //uboOffscreen.proj[1][1] *= -1; // Invert the Y axis for Vulkan
	mat4_translate(uboOffscreen.model, identity, (float[]){-lightPos[0], -lightPos[1], -lightPos[2]});
	memcpy(uboOffscreen.lightPos, lightPos, sizeof(lightPos));
	memcpy(uniformBufferOffscreen.pMappedData, &uboOffscreen, sizeof(uboOffscreen));
}

static void updateGeometry(const sharedBuffer buffer){
	vertices.n = map.vertexNum;
	indices.n = map.indexNum;
	createPlayerSphere(buffer.playerModel, &vertices, &indices);
	for(int i = 0; i < buffer.cuboids.n; i++){
		createCuboid(((obj3d*)buffer.cuboids.array)[i], &vertices, &indices);
	}
	for(int i = 0; i < buffer.ellipsoids.n; i++){
		createEllipsoid(((obj3d*)buffer.ellipsoids.array)[i], &vertices, &indices);
	}
	for(int i = 0; i < buffer.ellipsoidCylinders.n; i++){
		createEllipticCylinder(((obj3d*)buffer.ellipsoidCylinders.array)[i], &vertices, &indices);
	}
	vectorCheckCapacity(&vertices);
	vectorCheckCapacity(&indices);

}

void presentImage(const sharedBuffer buffer){

	vkWaitForFences(device, 1, &sync.fences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &sync.fences[currentFrame]);
	
	imageIndex = acquireNextImage(device, swapchain.swapchain, UINT64_MAX, sync.semaphores.wait[currentFrame], VK_NULL_HANDLE);


	updateGeometry(buffer);
	recordCommandBuffers();

	updateOffScreenUniformBuffer();
	updateUniformBuffers(buffer);

	VkPipelineStageFlags pipelineStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo = createSubmitInfo(&sync.semaphores.wait[currentFrame], &command.buffers[currentFrame], &sync.semaphores.signal[currentFrame], &pipelineStage);
	vkQueueSubmit(queue.drawing, 1, &submitInfo, sync.fences[currentFrame]);

	VkPresentInfoKHR presentInfo = createPresentInfoKHR(&sync.semaphores.signal[currentFrame], &swapchain.swapchain, &imageIndex);
	vkQueuePresentKHR(queue.presenting, &presentInfo);
	currentFrame = (currentFrame + 1) % swapchain.imageNum;
}

void recreateSwapChain(GLFWwindow *pWindow, int width, int height){
    // Wait for the device to finish before recreating the swap chain
	vkDeviceWaitIdle(device);
	// Wait if the window is minimized
    while (width == 0 || height == 0) {
        glfwWaitEvents();
		glfwGetFramebufferSize(pWindow, &width, &height);
    }

	int old_imageNum = swapchain.imageNum;
	deleteSwapchainAttachment(device, &swapchain);
	swapchain = createSwapchainAttachment(device, physicalDevice, surface, pWindow, imageArrayLayers, queue.drawingMode);

	deleteScenePass(device, &scenePass, old_imageNum);
	scenePass = createScenePass(device, physicalDevice, swapchain.surfaceFormat.format, depthFormat, msaaSamples, swapchain.extent, swapchain.imageViews, swapchain.imageNum);
	
	pipes.scene.scissor = configureScissor(swapchain.extent);
	pipes.scene.viewport = configureViewport(swapchain.extent);
}

void initVulkan(GLFWwindow *pWindow){
	instance = createInstance();
	physicalDevice = getBestPhysicalDevice(instance);

	uint32_t queueFamilyNumber = getqueueFamilyNumber(physicalDevice);
	VkQueueFamilyProperties *queueFamilyProperties = getQueueFamilyProperties(physicalDevice, queueFamilyNumber);
	uint32_t bestGraphicsQueueFamilyindex = getBestGraphicsQueueFamilyindex(queueFamilyProperties, queueFamilyNumber);
	
	device = createDevice(physicalDevice, queueFamilyNumber, queueFamilyProperties);
	queue = createQueueAttachment(device, queueFamilyProperties, bestGraphicsQueueFamilyindex);
	surface = createSurface(pWindow, instance, physicalDevice, bestGraphicsQueueFamilyindex);

	deleteQueueFamilyProperties(&queueFamilyProperties);

	swapchain = createSwapchainAttachment(device, physicalDevice, surface, pWindow, imageArrayLayers, queue.drawingMode);
	printf("frames in flight: %d\n", swapchain.imageNum);
	msaaSamples = getMaxUsableSampleCount(physicalDevice);
	depthFormat = findDepthFormat(physicalDevice);
	command = createCommandAttachment(device, bestGraphicsQueueFamilyindex, swapchain.imageNum);
	scenePass = createScenePass(device, physicalDevice, swapchain.surfaceFormat.format, depthFormat, msaaSamples, swapchain.extent, swapchain.imageViews, swapchain.imageNum);
	offScreenPass = createOffScreenPass(device, physicalDevice, depthFormat, swapchain.surfaceFormat.format, shadowMapResolution, command.pool, queue.drawing);
	uniformBuffers = createSceneUniformBuffers(device, physicalDevice, swapchain.imageNum);
	uniformBufferOffscreen = createOffScreenUniformBuffer(device, physicalDevice);

	VkBuffer *ubos = malloc(swapchain.imageNum * sizeof(VkBuffer));
	for(uint32_t i = 0; i < swapchain.imageNum; i++){
		ubos[i] = uniformBuffers[i].buffer.buffer;
	}
	descriptor = createDescriptors(device, swapchain.imageNum, offScreenPass.shadowMap.color.view, offScreenPass.shadowMap.sampler, uniformBufferOffscreen.buffer.buffer, ubos);
	free(ubos);

	pipes = createPipelines(device, scenePass.renderPass, offScreenPass.renderPass, msaaSamples, &descriptor.layout, swapchain.extent, shadowMapResolution);
	sync = createSyncObjects(device, swapchain.imageNum);
	map = initMap(&vertices, &indices);
	buffers = createDynamicBuffers(device, physicalDevice, indices, vertices, queue.drawing, command.pool, swapchain.imageNum);
}
	
void deleteVulkan(){
	vkDeviceWaitIdle(device);
	
	deleteSyncObjects(device, &sync, swapchain.imageNum);

	deleteCommandAttachment(device, &command, swapchain.imageNum);
	deletePipelines(device, &pipes);
	deleteDescriptors(device, &descriptor);
	deleteMappedBuffers(device, uniformBuffers, swapchain.imageNum);
	deleteBuffer(device, &uniformBufferOffscreen.buffer);
	deleteDynamicBuffers(device, &buffers, swapchain.imageNum);
	deleteOffScreenPass(device, &offScreenPass);
	deleteScenePass(device, &scenePass, swapchain.imageNum);
	deleteSwapchainAttachment(device, &swapchain);
	deleteSurface(instance, &surface);
	deleteDevice(&device);
	deleteInstance(&instance);
	free(vertices.array);
	free(indices.array);
}