#ifndef VK_FUN_H
#define VK_FUN_H

#include "std_c.h"
#include "ext.h"
#include "utils.h"
#include "shared_buffer.h"

#define ELLIPSOIDDETAIL 10
#define VerticesPerEllipsoid ELLIPSOIDDETAIL * ELLIPSOIDDETAIL * 2 - 4 * ELLIPSOIDDETAIL + 2
#define IndicesPerEllipsoid ELLIPSOIDDETAIL * ELLIPSOIDDETAIL * 12 - 12 * ELLIPSOIDDETAIL - 6
#define VerticesPerCube 24
#define IndicesPerCube 36
#define VerticesPerEllipticCylinder 2 + 4 * ELLIPSOIDDETAIL
#define IndicesPerEllipticCylinder 12 * ELLIPSOIDDETAIL

typedef struct MapSize {
    uint32_t vertexNum;
    uint32_t indexNum;
} mapSize;

typedef struct Vertex {
    float pos[3];
    float color[3];
    float normal[3];
} vertex_t;

typedef struct UniformDataOffscrene {
    float proj[4][4];
    float view[4][4];
    float model[4][4];
    float lightPos[4];
} uniformDataOffscreen;

typedef struct UniformDataScene {
    float proj[4][4];
    float view[4][4];
    float model[4][4];
    float lightPos[4];
} uniformDataScene;

typedef struct VkBufferandMemory {
    VkBuffer buffer;
    VkDeviceMemory memory;
} VkBufferandMemory;

typedef struct MappedBuffer {
    VkBufferandMemory buffer;
    void *pMappedData;
} mappedBuffer;

typedef struct VertexAndIndexBuffers {
    VkBufferandMemory vertex;
    VkBufferandMemory index;
    uint32_t indexBufferSize;
    uint32_t vertexBufferSize;
} vertexAndIndexBuffers;

typedef struct StagingBufferAttachment{
    mappedBuffer index;
    mappedBuffer vertex;
    uint32_t indexBufferSize;
    uint32_t vertexBufferSize;
} stagingBufferAttachment;

typedef struct DynamicBuffers{
    vertexAndIndexBuffers *buffers;
    stagingBufferAttachment *staging;
} dynamicBuffers;

typedef struct VkImageandMemory {
    VkImage image;
    VkDeviceMemory memory;
} VkImageandMemory;

typedef struct FrameBufferAttachment {
    VkImageandMemory image;
    VkImageView view;
} frameBufferAttachment;

typedef struct ShadowCubeMap {
    frameBufferAttachment color;
    VkSampler sampler;
    VkImageView *ImageViews;
} shadowCubeMap;

typedef struct OffScreenRenderPassAttachment {
    VkFramebuffer *frameBuffers;
    frameBufferAttachment depth;
    VkRenderPass renderPass;
    shadowCubeMap shadowMap;
    VkRenderPassBeginInfo *beginInfos;
    VkClearValue *clearValues;
} offScreenRenderPassAttachment;

typedef struct SceneRenderPassAttachment {
    VkFramebuffer *frameBuffers;
    frameBufferAttachment color;
    frameBufferAttachment depth;
    VkRenderPass renderPass;
    VkRenderPassBeginInfo *beginInfos;
    VkClearValue *clearValues;
} sceneRenderPassAttachment;

typedef struct SwapchainAttachment {
    VkImage *images;
    VkImageView *imageViews;
    uint32_t imageNum;
    VkSwapchainKHR swapchain;
    VkExtent2D extent;
    VkSurfaceFormatKHR surfaceFormat;
} swapchainAttachment;

typedef struct DescriptorSets {
    VkDescriptorSet offscreen;
    VkDescriptorSet *sceneSets;
} descriptorSets;

typedef struct Descriptors {
    descriptorSets sets;
    VkDescriptorSetLayout layout;
    VkDescriptorPool pool;
} descriptors;

typedef struct QueueAttachment {
    VkQueue drawing;
    uint32_t drawingMode;
    VkQueue presenting;
} queueAttachment;

typedef struct DepthBias{
    float constant;
    float clamp;
    float slope;
} depthBias;

typedef struct ScenePipe {
    VkPipeline pipe;
    VkPipelineLayout layout;
    VkRect2D scissor;
    VkViewport viewport;
} scenePipe;

typedef struct OffScreenPipe {
    VkPipeline pipe;
    VkPipelineLayout layout;
    VkRect2D scissor;
    VkViewport viewport;
    depthBias bias;
} offScreenPipe;

typedef struct Pipelines {
    scenePipe scene;
    offScreenPipe offscreen;
} pipelines;

typedef struct CommandAttachment {
    VkCommandBuffer *buffers;
    VkCommandPool pool;
    VkCommandBufferBeginInfo beginInfo;
} commandAttachment;

typedef struct SemaphoresAttachment {
    VkSemaphore *signal;
    VkSemaphore *wait;
} semaphoresAttachment;

typedef struct SyncObjects {
    semaphoresAttachment semaphores;
    VkFence *fences;
} syncObjects;

VkInstance createInstance();
void deleteInstance(VkInstance *pInstance);

VkPhysicalDevice getBestPhysicalDevice(const VkInstance instance);
VkSampleCountFlagBits getMaxUsableSampleCount(const VkPhysicalDevice physicalDevice);

VkDevice createDevice(const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyNumber, const VkQueueFamilyProperties *queueFamilyProperties);
void deleteDevice(VkDevice *pDevice);

uint32_t getqueueFamilyNumber(const VkPhysicalDevice physicalDevice);
VkQueueFamilyProperties *getQueueFamilyProperties(const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyNumber);
uint32_t getBestGraphicsQueueFamilyindex(const VkQueueFamilyProperties *pQueueFamilyProperties, const uint32_t queueFamilyNumber);
void deleteQueueFamilyProperties(VkQueueFamilyProperties **ppQueueFamilyProperties);
queueAttachment createQueueAttachment(const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyNumber, const VkQueueFamilyProperties *queueFamilyProperties, const uint32_t bestGraphicsQueueFamilyindex);
VkPresentInfoKHR createPresentInfoKHR(const VkSemaphore *pWaitSemaphores, const VkSwapchainKHR *pSwapchain, const uint32_t *pImageIndex);
VkSubmitInfo createSubmitInfo(const VkSemaphore *pWaitSemaphores, const VkCommandBuffer *pCommandBuffer, const VkSemaphore *pSignalSemaphores, const VkPipelineStageFlags *pPipelineStage);

VkSurfaceKHR createSurface(GLFWwindow *pWindow, const VkInstance instance, const VkPhysicalDevice physicalDevice, const uint32_t graphicsQueueFamilyindex);
void deleteSurface(const VkInstance instance, VkSurfaceKHR *pSurface);

swapchainAttachment createSwapchainAttachment(const VkDevice device, const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, GLFWwindow *pWindow, const uint32_t imageArrayLayers, const uint32_t graphicsQueueMode);
void deleteSwapchainAttachment(const VkDevice device, swapchainAttachment *pAttachment);
uint32_t acquireNextImage(const VkDevice device, const VkSwapchainKHR swapchain, const uint64_t timeout, const VkSemaphore semaphore, const VkFence fence);

VkImageView createImageView(const VkDevice device, const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags, const VkImageViewType viewType, const uint32_t layerCount, const uint32_t baseArrayLayer);
void transferImageLayout(const VkDevice device, const VkCommandPool commandPool, const VkQueue drawingQueue, const VkImage image, const uint32_t layerCount, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage, VkImageAspectFlags aspectMask);
frameBufferAttachment createFrameBufferAttachment(const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkMemoryPropertyFlags properties, const VkSampleCountFlagBits numSamples, const VkImageAspectFlags aspectFlags, const uint32_t arrayLayers, const VkImageCreateFlags imageFlags, const VkImageViewType viewType);
void deleteFrameBufferAttachment(const VkDevice device, frameBufferAttachment *pAttachment);
VkSampler createSampler(const VkDevice device, const VkFilter filter, const VkSamplerAddressMode addressMode, const VkCompareOp compareOp);
void deleteSampler(const VkDevice device, VkSampler *pSampler);
VkImageView *createShadowCubeMapFaceImageViews(const VkDevice device, const VkImage image, const VkFormat format);
void deleteShadowCubeMapFaceImageViews(const VkDevice device, VkImageView *pImageViews);

sceneRenderPassAttachment createScenePass(const VkDevice device, const VkPhysicalDevice physicalDevice, const VkFormat surfaceFormat, const VkFormat depthFormat, const VkSampleCountFlagBits numSamples, const VkExtent2D extent, const VkImageView *swapchainImageViews, const uint32_t imageViewNumber);
void deleteScenePass(const VkDevice device, sceneRenderPassAttachment *pPass, const uint32_t imageViewNumber);
offScreenRenderPassAttachment createOffScreenPass(const VkDevice device, const VkPhysicalDevice physicalDevice, const VkFormat depthFormat, const VkFormat colorFormat, const uint32_t shadowMapResolution, const VkCommandPool commandPool, const VkQueue drawingQueue);
void deleteOffScreenPass(const VkDevice device, offScreenRenderPassAttachment *pPass);

VkShaderModule getShader(const VkDevice device, const char *fileName);
void deleteShader(const VkDevice device, VkShaderModule *pShaderModule);

descriptors createDescriptors(const VkDevice device, const uint32_t maxFrames, const VkImageView shadowMapImageView, const VkSampler shadowMapSampler, const VkBuffer OffscreenBuffer, const VkBuffer *uniformBuffers);
void deleteDescriptors(const VkDevice device, descriptors *pDescriptors);

pipelines createPipelines(const VkDevice device, const VkRenderPass sceneRenderPass, const VkRenderPass offscreenRenderPass, const VkSampleCountFlagBits numSamples, const VkDescriptorSetLayout *pDescriptorSetLayout, const VkExtent2D sceneExtent, const uint32_t shadowMapResolution);    void deletePipelines(const VkDevice device, pipelines *pPipelines);
VkViewport configureViewport(const VkExtent2D extent);
VkRect2D configureScissor(const VkExtent2D extent);

VkCommandBuffer beginSingleTimeCommands(const VkDevice device, const VkCommandPool commandPool);
void endSingleTimeCommands(const VkDevice device, VkCommandBuffer *pCommandBuffer, const VkCommandPool commandPool, const VkQueue drawingQueue);
commandAttachment createCommandAttachment(const VkDevice device, const uint32_t queueFamilyIndex, const uint32_t commandBufferNumber);
void deleteCommandAttachment(const VkDevice device, commandAttachment *pCommand, const uint32_t commandBufferNumber);

syncObjects createSyncObjects(const VkDevice device, const uint32_t maxFrames);
void deleteSyncObjects(const VkDevice device, syncObjects *pSyncObjects, const uint32_t maxFrames);

//share vulkan file scope variables
    //main thread
    void initVulkan(GLFWwindow *pWindow);
    void recreateSwapChain(GLFWwindow *pWindow, int width, int height);
    void deleteVulkan();
    //rendering thread
    void presentImage(const sharedBuffer buffer);

void testLoop(GLFWwindow *window);

uint32_t findMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties, const VkPhysicalDevice physicalDevice);
void deleteBuffer(const VkDevice device, VkBufferandMemory *pBufferandMemory);
mappedBuffer *createSceneUniformBuffers(const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t maxFrames);
mappedBuffer createOffScreenUniformBuffer(const VkDevice device, const VkPhysicalDevice physicalDevice);
void deleteMappedBuffers(const VkDevice device, mappedBuffer *buffers, const uint32_t bufferNum);
dynamicBuffers createDynamicBuffers(const VkDevice device, const VkPhysicalDevice physicalDevice, const vec indices, const vec vertices, const VkQueue graphicsQueue, const VkCommandPool commandPool, const uint32_t frameNum);
void deleteDynamicBuffers(const VkDevice device, dynamicBuffers *pBuffers, const uint32_t frameNum);
void updateDynamicBuffers(dynamicBuffers *pBuffers, const vec indices, const vec vertices, const VkCommandBuffer commandBuffer, const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t currentFrame);

VkFormat findDepthFormat(const VkPhysicalDevice physicalDevice);
VkBool32 formatIsFilterable(const VkPhysicalDevice physicalDevice, const VkFormat format, const VkImageTiling tiling);

mapSize initMap(vec *pVertices, vec *pIndices);
void createEllipsoid(obj3d obj, vec *pVertices, vec *pIndices);
void createCuboid(obj3d obj, vec *pVertices, vec *pIndices);
void createEllipticCylinder(obj3d obj, vec *pVertices, vec *pIndices);
void createPlayerSphere(obj3d obj, vec *pVertices, vec *pIndices);
#endif
